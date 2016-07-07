/**
 * @file lock_win32.c
 * @brief Lock support for Win32 API
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
#include <malloc.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <osal_core.h>

struct os_lock
{
   SRWLOCK handle;
};

struct os_lock* os_createLock(void)
{
   struct os_lock* lock = malloc(sizeof(struct os_lock));
   if (lock)
   {
      InitializeSRWLock(&lock->handle);
   }
   return lock;
}

void os_destroyLock(struct os_lock* lock)
{
   if (lock)
   {
      free(lock);
   }
}

void os_lock(struct os_lock* lock)
{
   assert(lock);
   AcquireSRWLockExclusive(&lock->handle);
}

void os_unlock(struct os_lock* lock)
{
   assert(lock);
   ReleaseSRWLockExclusive(&lock->handle);
}

