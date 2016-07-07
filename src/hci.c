/**
 * @file hci.c
 * @brief HCI command implementation
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

#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include <osal_core.h>
#include <utils.h>

#include <hci.h>
#include <commands.h>

#include "hci_priv.h"
#include "lb_priv.h"

struct hci_condition
{
   struct os_condition* handle;

   uint16_t opcode;
   uint8_t capacity;
   uint8_t* buffer;

   uint8_t status;
   uint8_t length;
};

static struct hci_condition pendingCommands[4];

static struct os_lock* pendingLock;

void hci_initialize(void)
{
   memset(pendingCommands, 0, sizeof(pendingCommands));

   for (uint32_t ii = 0; ii < (sizeof(pendingCommands) / sizeof(pendingCommands[0])); ii ++)
   {
      pendingCommands[ii].handle = os_createCondition();
   }

   pendingLock = os_createLock();
}

void hci_cleanup(void)
{
   for (uint32_t ii = 0; ii < (sizeof(pendingCommands) / sizeof(pendingCommands[0])); ii ++)
   {
      os_destroyCondition(pendingCommands[ii].handle);
   }

   os_destroyLock(pendingLock);
}

void hci_on_eventCommandComplete(struct LB_Controller* controller, const struct HCI_EVENT_Command_Complete* event, uint8_t length)
{
   /*
    * signal event->opcode
    */
   signalCondition(uint16Value(&event->opcode), event->status, (((uint8_t*) event) + sizeof(struct HCI_EVENT_Command_Complete)), length - sizeof(struct HCI_EVENT_Command_Complete));
}

void hci_on_eventCommandStatus(struct LB_Controller* controller, const struct HCI_EVENT_Command_Status* event, uint8_t length)
{
   signalCondition(uint16Value(&event->opcode), event->status, NULL, 0);
}

void hci_on_vendorSpecificEvent(struct LB_Controller* controller, struct HCI_EVENT_Vendor_Specific* event, uint8_t length)
{
   uint16_t eventCode = uint16Value(&event->eventCode);

   if (lbDebugLevel > 1000)
   {
      printf("@ Vendor specific event: (code: %04x, reason: %02x)\n", (unsigned) eventCode, event->reasonCode);
      fflush(stdout);
   }
}

struct hci_condition* allocateCondition(uint16_t opcode, uint8_t* result, uint8_t capacity)
{
   os_lock(pendingLock);

   uint32_t ii = 0;

   while ((sizeof(pendingCommands) / sizeof(pendingCommands[0])) > ii )
   {
      if (0 == pendingCommands[ii].opcode)
      {
         // reserve
         pendingCommands[ii].opcode = opcode;
         break;
      }
      else
      {
         ii ++;
      }
   }

   os_unlock(pendingLock);

   if ((sizeof(pendingCommands) / sizeof(pendingCommands[0])) == ii)
   {
      return NULL;
   }

   struct hci_condition* cond = &pendingCommands[ii];

   os_resetCondition(cond->handle);

   cond->capacity = capacity;
   cond->buffer   = result;

   cond->length   = 0;
   cond->status   = HCI_CONTROLLER_BUSY;

   return cond;
}

void releaseCondition(uint16_t opcode)
{
   uint32_t ii = 0;

   while ((sizeof(pendingCommands) / sizeof(pendingCommands[0])) > ii)
   {
      if (opcode == pendingCommands[ii].opcode)
      {
         pendingCommands[ii].opcode = 0;
         break;
      }
      else
      {
         ii ++;
      }
   }

   assert ((sizeof(pendingCommands) / sizeof(pendingCommands[0])) != ii);
}

void signalCondition(uint16_t opcode, enum HCI_StatusCode status, const uint8_t* result, uint8_t length)
{
   if (lbDebugLevel > 1000)
   {
      printf("Result for opcode %04x: %02x", (unsigned) opcode, (unsigned) status);
      if (length)
      {
         putchar(' ');
         putchar(' ');
         utl_printBuffer(result, length);
      }
      putchar('\n');
   }

   uint32_t ii = 0;

   while ((sizeof(pendingCommands) / sizeof(pendingCommands[0])) > ii)
   {
      if (opcode == pendingCommands[ii].opcode)
      {
         break;
      }
      else
      {
         ii ++;
      }
   }

   if ((sizeof(pendingCommands) / sizeof(pendingCommands[0])) != ii)
   {
      struct hci_condition* cond = &pendingCommands[ii];

      if (length > cond->capacity)
      {
         length = cond->capacity;
      }

      if (length)
      {
         memcpy(cond->buffer, result, length);
      }

      cond->length = length;
      cond->status = status;

      os_signalCondition(cond->handle, NULL);
   }
   else
   {
      assert(false);
   }
}

enum HCI_StatusCode waitForCondition(struct hci_condition* cond, uint8_t* length)
{
   enum HCI_StatusCode retval = HCI_CONTROLLER_BUSY;

   void* arg;
   bool signaled = os_waitForCondition(cond->handle, 1000, &arg);
   if (signaled)
   {
      retval       = cond->status;
      *length      = cond->length;
      cond->opcode = 0;
   }

   return retval;
}

void hci_on_ATT_READ_BY_GROUP_TYPE_RESP_EVENT(struct LB_Controller* controller, const uint8_t* buffer, uint8_t length)
{
   assert(sizeof(struct Event_ATT_READ_BY_GROUP_TYPE_RESP) <= length);
   const struct Event_ATT_READ_BY_GROUP_TYPE_RESP* event = (const struct Event_ATT_READ_BY_GROUP_TYPE_RESP*) buffer;

   uint16_t connectionHandle = uint16Value(&event->connectionHandle);
   assert((0x06 == event->attributeDataLength) || (0x014 == event->attributeDataLength));
   assert(0 == (event->eventDataLength - 1) % event->attributeDataLength);
   assert((sizeof(struct Event_ATT_READ_BY_GROUP_TYPE_RESP) + event->eventDataLength - 1) == length);

   const uint8_t* attributeValue    = buffer + sizeof(struct Event_ATT_READ_BY_GROUP_TYPE_RESP);
   const uint8_t* const attributeValueEnd = buffer + length;

   while (attributeValueEnd > attributeValue)
   {
      uint16_t attributeHandle = attributeValue[0] | (((uint16_t) attributeValue[1]) << 8);
      uint16_t endGroupHandle  = attributeValue[2] | (((uint16_t) attributeValue[3]) << 8);

      struct LB_Device* device = getDevice(controller, connectionHandle);
      lb_on_discoveredPrimaryService(device, attributeHandle, endGroupHandle, attributeValue + 4, event->attributeDataLength - 4);

      attributeValue += event->attributeDataLength;
   }
}
