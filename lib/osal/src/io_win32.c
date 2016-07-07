/**
 * @file io_win32.c
 * @brief Serial port communication implementation for Win32
 * @author Florin Iucha <florin@signbit.net>
 * @copyright Apache License, Version 2.0
 */

/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of lightBLUE OSAL library
 */

/**
 * @privatesection
 */

#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#include <osal_core.h>
#include <osal_serial.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

#define THREAD_SHUTDOWN_KEY 0

static volatile bool interrupted = false;

static HANDLE sleepPort = INVALID_HANDLE_VALUE;
static HANDLE completionPort = INVALID_HANDLE_VALUE;
static HANDLE shutdownCompletePort = INVALID_HANDLE_VALUE;

static BOOL WINAPI consoleHandler(DWORD dwType)
{
   switch (dwType)
   {
         // intentional fall-through
      case CTRL_C_EVENT:
      case CTRL_BREAK_EVENT:

         interrupted = true;
         os_on_shutdownRequested();
         //PostQueuedCompletionStatus(completionPort, 0, THREAD_SHUTDOWN_KEY, NULL);
         PostQueuedCompletionStatus(sleepPort, 0, THREAD_SHUTDOWN_KEY, NULL);
         break;

      default:
         break;
   }
   return TRUE;
}

static volatile uint32_t ioDebugLevel = 0;

void io_setDebugLevel(uint32_t value)
{
   ioDebugLevel = value;
}

struct io_channel
{
   HANDLE      serialHandle;

   OVERLAPPED  readOverlapped;
   uint8_t     buffer[256];

   OVERLAPPED  writeOverlapped;

   SRWLOCK              writeVariablesLock;
   CONDITION_VARIABLE   allWritesHaveCompleted;
   uint32_t             writeScheduled;
   uint32_t             writeCompleted;

   void*       userData;
};

void* io_getUserPtr(struct io_channel* channel)
{
   return channel->userData;
}

void __attribute__((weak)) io_on_transmissionComplete(struct io_channel* channel)
{
}

static void ioThreadHandler(void* argument)
{
   (void) argument;

   while (true)
   {
      DWORD byteCount = 0;
      ULONG_PTR key = NULL;
      LPOVERLAPPED overlappedPtr;

      BOOL successful = GetQueuedCompletionStatus(completionPort, &byteCount, &key, &overlappedPtr, INFINITE);
      if (successful)
      {
         if (THREAD_SHUTDOWN_KEY == key)
         {
            assert(NULL == overlappedPtr);
            if (ioDebugLevel > 1000)
            {
               puts("I/O Thread interrupted");
            }
            break;
         }
         else
         {
            struct io_channel* channel = (struct io_channel*) key;

            if (&channel->readOverlapped == overlappedPtr)
            {
               if (byteCount)
               {
                  if (ioDebugLevel > 1000)
                  {
                     printf("& Read %lu bytes\n", byteCount);
                     fflush(stdout);
                  }
                  assert(sizeof(channel->buffer) >= byteCount);

                  io_on_dataReceived(channel, channel->buffer, byteCount);
               }

               memset(&channel->readOverlapped, 0, sizeof(channel->readOverlapped));

               DWORD bytesRead = 0;
               BOOL restartRead = ReadFile(channel->serialHandle, channel->buffer, sizeof(channel->buffer), &bytesRead, &channel->readOverlapped);
               if (! restartRead)
               {
                  DWORD err = GetLastError();
                  if (ERROR_IO_PENDING != err)
                  {
                     if (ioDebugLevel)
                     {
                        printf("!! ReadFile error code: %lu\n", err);
                     }
                  }
               }
               else
               {
                  if (ioDebugLevel)
                  {
                     printf("!! ReadFile success! Received %lu bytes.\n", bytesRead);
                  }
               }
            }
            else
            {
               assert(&channel->writeOverlapped == overlappedPtr);
               if (ioDebugLevel > 1000)
               {
                  printf("& Wrote %lu bytes\n", byteCount);
                  fflush(stdout);
               }

               AcquireSRWLockExclusive(&channel->writeVariablesLock);

               channel->writeCompleted += byteCount;
               if (channel->writeCompleted == channel->writeScheduled)
               {
                  io_on_transmissionComplete(channel);

                  WakeConditionVariable(&channel->allWritesHaveCompleted);
               }

               ReleaseSRWLockExclusive(&channel->writeVariablesLock);
            }
         }
      }
      else
      {
         DWORD err = GetLastError();
         if (ioDebugLevel > 1000)
         {
            printf("!! io thread: GetQueuedCompletionStatus error: %lu\n", err);
            fflush(stdout);
         }
      }
   }

   PostQueuedCompletionStatus(shutdownCompletePort, 0, THREAD_SHUTDOWN_KEY, NULL);

   _endthread();
}

static uintptr_t ioThreadId = -1;

int os_initialize(void)
{
   completionPort = CreateIoCompletionPort(
         INVALID_HANDLE_VALUE,            // file handle
         NULL,                            // existing completion port
         0,                               // completion key
         1                                // number of concurrent threads
         );

   if (INVALID_HANDLE_VALUE == completionPort)
   {
      return -1;
   }

   sleepPort = CreateIoCompletionPort(
         INVALID_HANDLE_VALUE,            // file handle
         NULL,                            // existing completion port
         0,                               // completion key
         1                                // number of concurrent threads
         );

   if (INVALID_HANDLE_VALUE == sleepPort)
   {
      return -1;
   }

   shutdownCompletePort = CreateIoCompletionPort(
         INVALID_HANDLE_VALUE,            // file handle
         NULL,                            // existing completion port
         0,                               // completion key
         1                                // number of concurrent threads
         );

   if (INVALID_HANDLE_VALUE == shutdownCompletePort)
   {
      return -1;
   }

   interrupted = false;
   if (! SetConsoleCtrlHandler((PHANDLER_ROUTINE) consoleHandler, TRUE))
   {
      return -2;
   }

   ioThreadId = _beginthread(ioThreadHandler, 0, NULL);
   if (-1L == ioThreadId)
   {
      return -3;
   }

   return 0;
}

void os_cleanup(void)
{
   PostQueuedCompletionStatus(completionPort, 0, THREAD_SHUTDOWN_KEY, NULL);
   PostQueuedCompletionStatus(sleepPort, 0, THREAD_SHUTDOWN_KEY, NULL);

   // wait until all threads are shut down
   {
      DWORD byteCount = 0;
      ULONG_PTR key = NULL;
      LPOVERLAPPED overlappedPtr;

      BOOL status = GetQueuedCompletionStatus(shutdownCompletePort, &byteCount, &key, &overlappedPtr, INFINITE);
      if (status)
      {
         assert(THREAD_SHUTDOWN_KEY == key);
         assert(NULL == overlappedPtr);
      }
   }

   CloseHandle(completionPort);
   CloseHandle(sleepPort);
   CloseHandle(shutdownCompletePort);
}

static const char WINDOWS_SERIAL_MAGIC[] = "\\\\.\\";

struct io_channel* io_openSerialPort(const char* portName, uint32_t baudRate, void* userData)
{
   struct io_channel* channel = (struct io_channel*) malloc(sizeof(struct io_channel));
   if (! channel)
   {
      return NULL;
   }

   memset(channel, 0, sizeof(*channel));
   channel->userData = userData;

   channel->serialHandle = INVALID_HANDLE_VALUE;

   char comport[128];
   memcpy(comport, WINDOWS_SERIAL_MAGIC, sizeof(WINDOWS_SERIAL_MAGIC) - 1);
   strncpy(comport + sizeof(WINDOWS_SERIAL_MAGIC) - 1, portName, sizeof(comport) - 20);
   HANDLE serialHandle = CreateFileA(
      comport,
      GENERIC_READ | GENERIC_WRITE,
      0,                               // exclusive
      NULL,                            // no security context
      OPEN_EXISTING,
      FILE_FLAG_OVERLAPPED,
      NULL                             // template
      );

   if (INVALID_HANDLE_VALUE == serialHandle)
   {
      if (ioDebugLevel)
      {
         DWORD err = GetLastError();
         printf("!! Failed to open port: %lu\n", err);
      }
      goto done;
   }

   DCB dcb;

   SecureZeroMemory(&dcb, sizeof(DCB));
   dcb.DCBlength = sizeof(DCB);

   if (! GetCommState(serialHandle, &dcb))
   {
      if (ioDebugLevel)
      {
         DWORD err = GetLastError();
         printf("!! Failed to get comm state: %lu\n", err);
      }
      goto done;
   }

   dcb.BaudRate     = baudRate;
   dcb.ByteSize     = 8;
   dcb.Parity       = NOPARITY;
   dcb.StopBits     = ONESTOPBIT;

   dcb.fOutX        = FALSE;
   dcb.fInX         = FALSE;
   dcb.fNull        = FALSE;

   dcb.fOutxCtsFlow = FALSE;
   dcb.fOutxDsrFlow = FALSE;
   dcb.fDtrControl  = DTR_CONTROL_DISABLE;
   dcb.fRtsControl  = RTS_CONTROL_DISABLE;

   if (! SetCommState(serialHandle, &dcb))
   {
      if (ioDebugLevel)
      {
         DWORD err = GetLastError();
         printf("!! Failed to set comm state: %lu\n", err);
      }
      goto done;
   }

   COMMTIMEOUTS commtimeouts;

   if (! GetCommTimeouts(serialHandle, &commtimeouts))
   {
      if (ioDebugLevel)
      {
         puts("!! Failed to get comm timeouts.");
      }
      goto done;
   }

   // disable timeouts
   commtimeouts.ReadIntervalTimeout         = MAXDWORD;
   commtimeouts.ReadTotalTimeoutMultiplier  = MAXDWORD;
   commtimeouts.ReadTotalTimeoutConstant    = 100000;
   commtimeouts.WriteTotalTimeoutMultiplier = 0;
   commtimeouts.WriteTotalTimeoutConstant   = 0;

   if (! SetCommTimeouts(serialHandle, &commtimeouts))
   {
      if (ioDebugLevel)
      {
         DWORD err = GetLastError();
         printf("!! Failed to set comm timeouts: %lu", err);
      }
      goto done;
   }

   if (! PurgeComm(serialHandle, PURGE_RXCLEAR | PURGE_TXCLEAR))
   {
      if (ioDebugLevel)
      {
         DWORD err = GetLastError();
         printf("!! Failed to clear comm buffer: %lu", err);
      }
      goto done;
   }

   DWORD errors = 0;
   COMSTAT comstat;
   memset(&comstat, 0, sizeof(comstat));

   if (! ClearCommError(serialHandle, &errors, &comstat))
   {
      if (ioDebugLevel)
      {
         DWORD err = GetLastError();
         printf("!! Failed to clear comm errors: %lu", err);
      }
      goto done;
   }

   HANDLE hh = CreateIoCompletionPort(
         serialHandle,                    // file handle
         completionPort,                  // existing completion port
         (ULONG_PTR) channel,             // completion key
         0                                // number of concurrent threads; ignored
         );

   if (NULL == hh)
   {
      goto done;
   }

   assert(completionPort == hh);

   // indicate that all is well
   channel->serialHandle = serialHandle;
   serialHandle          = INVALID_HANDLE_VALUE;

   channel->writeScheduled = 0;
   channel->writeCompleted = 0;

   InitializeSRWLock(&channel->writeVariablesLock);
   InitializeConditionVariable(&channel->allWritesHaveCompleted);

   // kickstart the read
   DWORD bytesRead = 0;
   BOOL startRead = ReadFile(channel->serialHandle, channel->buffer, sizeof(channel->buffer), &bytesRead, &channel->readOverlapped);
   if (! startRead)
   {
      DWORD err = GetLastError();
      if (ERROR_IO_PENDING != err)
      {
         if (ioDebugLevel)
         {
            printf("!! ReadFile error code: %lu\n", err);
         }
      }
      else
      {
         if (ioDebugLevel > 1000)
         {
            puts("!! ReadFile is pending");
         }
      }
   }
   else
   {
      if (ioDebugLevel)
      {
         printf("!! ReadFile success! Received %lu bytes.\n", bytesRead);
      }
   }

   if (ioDebugLevel > 1000)
   {
      printf("& Port %s open and ready\n", portName);
   }

done:

   if (INVALID_HANDLE_VALUE != serialHandle)
   {
      CloseHandle(serialHandle);
   }

   if (INVALID_HANDLE_VALUE == channel->serialHandle)
   {
      free(channel);
      channel = NULL;
   }

   return channel;
}

void io_closePort(struct io_channel* channel)
{
   if (channel)
   {
      CloseHandle(channel->serialHandle);
      free(channel);
   }
}

bool os_interrupted(void)
{
   return interrupted;
}

void os_sleep_ms(uint32_t duration_ms)
{
   DWORD byteCount = 0;
   ULONG_PTR key = NULL;
   LPOVERLAPPED overlappedPtr;

   BOOL status = GetQueuedCompletionStatus(sleepPort, &byteCount, &key, &overlappedPtr, duration_ms);
   if (status)
   {
      assert(THREAD_SHUTDOWN_KEY == key);
      assert(NULL == overlappedPtr);
   }
}

void io_sendData(struct io_channel* channel, const uint8_t* buffer, uint32_t length)
{
   assert(channel);
   assert(INVALID_HANDLE_VALUE != channel->serialHandle);

   memset(&channel->writeOverlapped, 0, sizeof(channel->writeOverlapped));

   AcquireSRWLockExclusive(&channel->writeVariablesLock);
   channel->writeScheduled += length;
   ReleaseSRWLockExclusive(&channel->writeVariablesLock);

   DWORD bytesWritten;
   BOOL status = WriteFile(channel->serialHandle, buffer, length, &bytesWritten, &channel->writeOverlapped);
   if (! status)
   {
      DWORD err = GetLastError();
      if (ERROR_IO_PENDING != err)
      {
         if (ioDebugLevel)
         {
            printf("!! WriteFile error code %lu\n", err);
         }
      }
   }
   else
   {
      assert(length == bytesWritten);
   }
}

void io_waitForTransmitComplete(struct io_channel* channel)
{
   AcquireSRWLockExclusive(&channel->writeVariablesLock);
   while (channel->writeCompleted != channel->writeScheduled)
   {
      SleepConditionVariableSRW(&channel->allWritesHaveCompleted, &channel->writeVariablesLock, INFINITE, 0);
   }
   ReleaseSRWLockExclusive(&channel->writeVariablesLock);
}

void __attribute__((weak)) io_on_dataReceived(struct io_channel* channel, const uint8_t* buffer, uint32_t length)
{
}

void __attribute__((weak)) os_on_shutdownRequested(void)
{
}

void os_waitForKeyboardInterrupt(void)
{
   DWORD byteCount = 0;
   ULONG_PTR key = NULL;
   LPOVERLAPPED overlappedPtr;

   BOOL status = GetQueuedCompletionStatus(sleepPort, &byteCount, &key, &overlappedPtr, INFINITE);
   if (status)
   {
      assert(THREAD_SHUTDOWN_KEY == key);
      assert(NULL == overlappedPtr);
   }
}

