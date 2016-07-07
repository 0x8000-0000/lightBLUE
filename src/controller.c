/**
 * @file controller.c
 * @brief HCI controller implementation
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
 * This file is part of LightBLUE Bluetooth Smart Library
 */

#include <malloc.h>
#include <string.h>

#include <osal_core.h>
#include <osal_serial.h>

#include <controller.h>

#include "lb_priv.h"
#include "hci_priv.h"

int lb_initialize(void)
{
   if (os_initialize() < 0)
   {
      return -1;
   }

   hci_initialize();

   return 0;
}

void lb_cleanup(void)
{
   hci_cleanup();

   os_cleanup();
}

struct LB_Controller* lb_connect(const char* portName)
{
   struct LB_Controller* controller = malloc(sizeof(struct LB_Controller));
   if (! controller)
   {
      return NULL;
   }

   memset(controller, 0, sizeof(struct LB_Controller));

   controller->operationLock     = os_createLock();
   controller->operationComplete = os_createCondition();

   for (uint32_t ii = 0; ii < sizeof(controller->device) / sizeof(controller->device[0]); ii ++)
   {
      controller->device[ii].controller       = 0;
      controller->device[ii].connectionHandle = INVALID_CONNECTION_HANDLE;
   }

   controller->channel = io_openSerialPort(portName, 115200, controller);
   if (! controller->channel)
   {
      lb_disconnect(controller);
      controller = NULL;
      goto done;
   }

done:

   return controller;
}

void lb_disconnect(struct LB_Controller* controller)
{
   if (controller)
   {
      for (uint32_t ii = 0; ii < sizeof(controller->device) / sizeof(controller->device[0]); ii ++)
      {
         if (INVALID_CONNECTION_HANDLE != controller->device[ii].connectionHandle)
         {
            // TODO: disconnect
         }
      }

      os_destroyLock(controller->operationLock);
      os_destroyCondition(controller->operationComplete);

      if (controller->channel)
      {
         io_closePort(controller->channel);
      }

      free(controller);
   }
}

