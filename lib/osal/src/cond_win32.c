/**
 * @file cond_win32.h
 * @brief Condition support for Win32 API
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

struct os_condition
{
   HANDLE handle;
   void* status;
};

struct os_condition* os_createCondition(void)
{
   struct os_condition* cond = malloc(sizeof(struct os_condition));

   cond->handle = CreateEvent(
         NULL,       // attributes
         true,       // manual event
         false,      // initial state
         NULL);      // name

   if (INVALID_HANDLE_VALUE == cond->handle)
   {
      free(cond);
      cond = NULL;
   }

   return cond;
}

void os_destroyCondition(struct os_condition* cond)
{
   if (cond)
   {
      CloseHandle(cond->handle);
      free(cond);
   }
}

void os_resetCondition(struct os_condition* cond)
{
   assert(cond);
   cond->status = NULL;
   ResetEvent(cond->handle);
}

void os_signalCondition(struct os_condition* cond, void* status)
{
   assert(cond);
   cond->status = status;
   SetEvent(cond->handle);
}

bool os_waitForCondition(struct os_condition* cond, uint32_t timeout_ms, void** status)
{
   assert(cond);
   DWORD ret = WaitForSingleObject(cond->handle, timeout_ms);

   if (WAIT_OBJECT_0 == ret)
   {
      *status = cond->status;
      return true;
   }
   else
   {
      return false;
   }
}

