/**
 * @file test_conditions.c
 * @brief Test the condition implementation
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
 * This file is part of serial_base library
 */

#include <assert.h>

#include <osal_core.h>

int main(void)
{
   os_initialize();

   struct os_condition* cond = os_createCondition();

   void* msg = NULL;

   bool signaled = os_waitForCondition(cond, 1000, &msg);

   assert(false == signaled);

   os_destroyCondition(cond);

   os_cleanup();

   return 0;
}
