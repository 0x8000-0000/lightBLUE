/**
 * @file commands.c
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

/*
 * Based on:
 *    Bluetooth Core Specification Version 4.2
 *    TI BLE Vendor Specific HCI Reference Guide Version 2.1.0
 *    ST BlueNRG Bluetooth LE stack application command interface (ACI) Revision 4
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <osal_core.h>

#include <hci.h>
#include <commands.h>
#include <utils.h>
#include <gap.h>

#include "lb_priv.h"
#include "hci_priv.h"

uint32_t lbDebugLevel = 0;

void lb_setDebugLevel(uint32_t level)
{
   lbDebugLevel = level;
}

#define DEBUG_PRINT_HCI_COMMANDS

static void processBuffer(struct LB_Controller* controller)
{
   uint8_t* buffer = controller->buffer;
   uint32_t length = controller->length;

   while (sizeof(struct HCI_EventHeader) <= length)
   {
      if (lbDebugLevel > 100)
      {
         printf("# Receive: ");
         utl_printBuffer(buffer, length);
         putchar('\n');
         fflush(stdout);
      }

      if (HCI_PACKET_EVENT == *buffer)
      {
         const struct HCI_EventHeader* header = (struct HCI_EventHeader*) buffer;

         const uint32_t eventLength = sizeof(struct HCI_EventHeader) + header->length;

         if (eventLength <= length)
         {
            const void* ptr = &buffer[sizeof(struct HCI_EventHeader)];

            // we have a full packet
            switch (header->opcode)
            {
               case HCI_EVENTID_Disconnection_Complete:
                  {
                     assert(sizeof(struct Event_HCI_DISCONNECTION_COMPLETE) == header->length);
                     const struct Event_HCI_DISCONNECTION_COMPLETE* linkEvent = (const struct Event_HCI_DISCONNECTION_COMPLETE*) ptr;
                     on_disconnectedFromDevice(controller, uint16Value(&linkEvent->connectionHandle), linkEvent->reason);
                  }
                  break;

               case HCI_EVENTID_Command_Complete:
                  hci_on_eventCommandComplete(controller, (const struct HCI_EVENT_Command_Complete*) ptr, header->length);
                  break;

               case HCI_EVENTID_Command_Status:
                  hci_on_eventCommandStatus(controller, (const struct HCI_EVENT_Command_Status*) ptr, header->length);
                  break;

               case HCI_EVENTID_Meta:
                  if (controller->vendorFunctions)
                  {
                     controller->vendorFunctions->on_metaEvent(controller, (const uint8_t*) ptr, header->length);
                  }
                  break;

               case HCI_EVENTID_Vendor_Specific:
                  if (controller->vendorFunctions)
                  {
                     controller->vendorFunctions->on_vendorSpecificEvent(controller, (const uint8_t*) ptr, header->length);
                  }
                  else
                  {
                     hci_on_vendorSpecificEvent(controller, (struct HCI_EVENT_Vendor_Specific*) ptr, header->length);
                  }
                  break;
            }

            buffer += eventLength;
            length -= eventLength;
         }
         else
         {
            // incomplete packet
            memmove(controller->buffer, buffer, length);
            break;
         }
      }
      else
      {
         // corrupt data
         memset(controller->buffer, 0, sizeof(controller->buffer));
         length = 0;
         break;
      }
   }

   controller->length = length;
}

void io_on_dataReceived(struct io_channel* channel, const uint8_t* buffer, uint32_t length)
{
   if (lbDebugLevel > 10000)
   {
      printf("# Received %u bytes\n", length);
      fflush(stdout);
   }

   struct LB_Controller* controller = (struct LB_Controller*) io_getUserPtr(channel);

   assert((length + controller->length) < sizeof(controller->buffer));

   memcpy(controller->buffer + controller->length, buffer, length);
   controller->length += length;

   processBuffer(controller);
}

enum LB_STATUS lb_executeCommand(struct LB_Controller* controller, const uint8_t* command, uint8_t commandLength, uint8_t* response, uint8_t maxResponseLength)
{
   assert(4 <= commandLength);
   uint16_t opcode = (((uint16_t) command[2]) << 8) | command[1];
   struct hci_condition* cond = allocateCondition(opcode, response, maxResponseLength);

   if (lbDebugLevel > 100)
   {
      printf("# Send: ");
      utl_printBuffer(command, commandLength);
      putchar('\n');
      fflush(stdout);
   }

   io_sendData(controller->channel, command, commandLength);

   io_waitForTransmitComplete(controller->channel);

   uint8_t responseLength = 0;
   enum HCI_StatusCode status = waitForCondition(cond, &responseLength);

   if (HCI_STATUS_SUCCESS == status)
   {
      return LB_OK;
   }
   else
   {
      return LB_FAILURE;
   }
}

MAKE_HCI_COMMAND(READ_LOCAL_VERSION_INFORMATION);

enum LB_STATUS lb_readLocalVersionInformation(struct LB_Controller* controller, struct HCI_RESPONSE_Read_Local_Version_Information* version)
{
   enum LB_STATUS status = lb_executeCommand(controller, (const uint8_t*) &CMD_READ_LOCAL_VERSION_INFORMATION, sizeof(CMD_READ_LOCAL_VERSION_INFORMATION), (uint8_t*) version, sizeof(*version));

   return status;
}

MAKE_HCI_COMMAND(RESET);

enum LB_STATUS lb_resetHCI(struct LB_Controller* controller)
{
   return lb_executeCommand(controller, (const uint8_t*) &CMD_RESET, sizeof(CMD_RESET), NULL, 0);
}

extern struct lb_vendorFunctions lb_vendorFunctions_ST;
extern struct lb_vendorFunctions lb_vendorFunctions_TI;

enum LB_STATUS lb_initializeHCI(struct LB_Controller* controller)
{
   enum LB_STATUS status = LB_OK;

   status = lb_resetHCI(controller);

   if (LB_OK == status)
   {
      struct HCI_RESPONSE_Read_Local_Version_Information version;

      status = lb_readLocalVersionInformation(controller, &version);

      if (LB_OK == status)
      {
         controller->manufacturerId = (unsigned int) uint16Value(&version.manufacturerId);
      }
   }

   switch (controller->manufacturerId)
   {
      case 0x0D:
         controller->vendorFunctions = &lb_vendorFunctions_TI;
         break;

      case 0x30:
         controller->vendorFunctions = &lb_vendorFunctions_ST;
         break;

      default:
         break;
   }

   if (controller->vendorFunctions)
   {
      status = controller->vendorFunctions->initializeHCI(controller);
   }
   else
   {
      status = LB_UNKNOWN_VENDOR;
      printf("%% Unknown HCI vendor %x\n", (unsigned) controller->manufacturerId);
   }

   return status;
}

enum LB_STATUS lb_configureAsCentral(struct LB_Controller* controller)
{
   enum LB_STATUS status = LB_UNKNOWN_VENDOR;

   if (controller->vendorFunctions)
   {
      status = controller->vendorFunctions->configureAsCentral(controller);
   }
   else
   {
      printf("%% Unknown HCI vendor %x\n", (unsigned) controller->manufacturerId);
   }

   return status;
}

enum LB_STATUS lb_startDeviceDiscovery(struct LB_Controller* controller)
{
   enum LB_STATUS status = LB_UNKNOWN_VENDOR;

   if (controller->vendorFunctions)
   {
      status = controller->vendorFunctions->startDeviceDiscovery(controller);
   }
   else
   {
      printf("%% Unknown HCI vendor %x\n", (unsigned) controller->manufacturerId);
   }

   return status;
}

enum LB_STATUS lb_stopDeviceDiscovery(struct LB_Controller* controller)
{
   enum LB_STATUS status = LB_UNKNOWN_VENDOR;

   if (controller->vendorFunctions)
   {
      status = controller->vendorFunctions->stopDeviceDiscovery(controller);
   }
   else
   {
      printf("%% Unknown HCI vendor %x\n", (unsigned) controller->manufacturerId);
   }

   return status;
}

enum LB_STATUS lb_openDeviceConnection(struct LB_Controller* controller, const uint8_t* address, struct LB_Device** device)
{
   os_lock(controller->operationLock);
   os_resetCondition(controller->operationComplete);

   enum LB_STATUS status = LB_UNKNOWN_VENDOR;

   if (controller->vendorFunctions)
   {
      status = controller->vendorFunctions->openDeviceConnection(controller, address);
   }
   else
   {
      printf("%% Unknown HCI vendor %x\n", (unsigned) controller->manufacturerId);
   }

   void* arg = NULL;
   bool signaled = os_waitForCondition(controller->operationComplete, 2000, &arg);
   if (! signaled)
   {
      *device = NULL;
      status = LB_OPERATION_TIMEOUT;
      goto done;
   }

   *device = (struct LB_Device*) arg;
   status = LB_OK;

done:

   os_unlock(controller->operationLock);

   return status;
}

void on_connectedToDevice(struct LB_Controller* controller, const uint8_t* address, uint16_t handle)
{
   struct LB_Device* device = NULL;

   uint32_t ii = 0;
   while (ii < (sizeof(controller->device) / sizeof(controller->device[0])))
   {
      if (INVALID_CONNECTION_HANDLE == controller->device[ii].connectionHandle)
      {
         device = &controller->device[ii];
         break;
      }
   }
   assert(device);

   device->controller       = controller;
   device->connectionHandle = handle;

   device->operationLock     = os_createLock();
   device->operationComplete = os_createCondition();

   os_signalCondition(controller->operationComplete, device);
}

enum LB_STATUS lb_closeDeviceConnection(struct LB_Device* device)
{
   assert(device);

   struct LB_Controller* controller = device->controller;

   os_lock(controller->operationLock);
   os_resetCondition(controller->operationComplete);

   enum LB_STATUS status = LB_UNKNOWN_VENDOR;

   if (controller->vendorFunctions)
   {
      status = controller->vendorFunctions->closeDeviceConnection(device);
   }
   else
   {
      printf("%% Unknown HCI vendor %x\n", (unsigned) controller->manufacturerId);
   }

   void* arg = NULL;
   bool signaled = os_waitForCondition(controller->operationComplete, 1000, &arg);
   if (! signaled)
   {
      status = LB_OPERATION_TIMEOUT;
      goto done;
   }

   status = LB_OK;

done:

   os_destroyCondition(device->operationComplete);
   os_destroyLock(device->operationLock);

   device->controller       = NULL;
   device->connectionHandle = INVALID_CONNECTION_HANDLE;

   os_unlock(controller->operationLock);

   return status;
}

void on_disconnectedFromDevice(struct LB_Controller* controller, uint16_t connectionHandle, uint8_t reason)
{
   struct LB_Device* device = getDevice(controller, connectionHandle);
   lb_on_disconnectedFromDevice(device, reason);

   os_signalCondition(controller->operationComplete, (void*) (uintptr_t) reason);
}

enum LB_STATUS lb_startServiceDiscovery(struct LB_Device* device)
{
   if (! isDeviceConnected(device))
   {
      return LB_DEVICE_NOT_CONNECTED;
   }

   struct LB_Controller* controller = device->controller;

   if (! controller->vendorFunctions)
   {
      printf("%% Unknown HCI vendor %x\n", (unsigned) controller->manufacturerId);
      return LB_UNKNOWN_VENDOR;
   }

   os_lock(device->operationLock);
   assert(0 == device->pendingOperation.attributeHandle);
   assert(PO_IDLE == device->pendingOperation.type);

   device->pendingOperation.type            = PO_DISCOVER;

   enum HCI_StatusCode status = HCI_HOST_TIMEOUT;

   os_resetCondition(device->operationComplete);

   enum LB_STATUS commandStatus = device->controller->vendorFunctions->startServiceDiscovery(device);
   if (LB_OK != commandStatus)
   {
      goto done;
   }

   void* operationStatus = NULL;
   bool signaled = os_waitForCondition(device->operationComplete, 10 * 1000, &operationStatus);
   if (! signaled)
   {
      status = HCI_HOST_TIMEOUT;
      goto done;
   }

   if (NULL == operationStatus)
   {
      status = HCI_STATUS_SUCCESS;
   }

done:

   device->pendingOperation.attributeHandle = 0;
   device->pendingOperation.type            = PO_IDLE;

   os_unlock(device->operationLock);

   if (HCI_STATUS_SUCCESS == status)
   {
      return LB_OK;
   }
   else
   {
      return LB_FAILURE;
   }
}

void on_serviceDiscoveryComplete(struct LB_Controller* controller, uint16_t connectionHandle)
{
   struct LB_Device* device = getDevice(controller, connectionHandle);

   os_signalCondition(device->operationComplete, 0);
}

enum LB_STATUS lb_writeCharValue(struct LB_Device* device, uint16_t attributeHandle, const uint8_t* attributeValue, uint8_t attributeLength)
{
   if (! isDeviceConnected(device))
   {
      return LB_DEVICE_NOT_CONNECTED;
   }

   struct LB_Controller* controller = device->controller;

   if (! controller->vendorFunctions)
   {
      printf("%% Unknown HCI vendor %x\n", (unsigned) controller->manufacturerId);
      return LB_UNKNOWN_VENDOR;
   }

   os_lock(device->operationLock);
   assert(0 == device->pendingOperation.attributeHandle);
   assert(PO_IDLE == device->pendingOperation.type);

   device->pendingOperation.attributeHandle = attributeHandle;
   device->pendingOperation.type            = PO_WRITE;

   enum HCI_StatusCode status = HCI_HOST_TIMEOUT;

   os_resetCondition(device->operationComplete);

   enum LB_STATUS commandStatus = controller->vendorFunctions->writeCharValue(device, attributeHandle, attributeValue, attributeLength);
   if (LB_OK != commandStatus)
   {
      goto done;
   }

   void* operationStatus = NULL;
   bool signaled = os_waitForCondition(device->operationComplete, 1000, &operationStatus);
   if (! signaled)
   {
      goto done;
   }

   if (NULL == operationStatus)
   {
      status = HCI_STATUS_SUCCESS;
   }

done:

   device->pendingOperation.attributeHandle = 0;
   device->pendingOperation.type            = PO_IDLE;

   os_unlock(device->operationLock);

   if (HCI_STATUS_SUCCESS == status)
   {
      return LB_OK;
   }
   else
   {
      return LB_FAILURE;
   }
}

enum LB_STATUS lb_readCharValue(struct LB_Device* device, uint16_t attributeHandle, uint8_t* attributeValue, uint8_t attributeCapacity, uint8_t* attributeLength)
{
   if (! isDeviceConnected(device))
   {
      return LB_DEVICE_NOT_CONNECTED;
   }

   struct LB_Controller* controller = device->controller;

   if (! controller->vendorFunctions)
   {
      printf("%% Unknown HCI vendor %x\n", (unsigned) controller->manufacturerId);
      return LB_UNKNOWN_VENDOR;
   }

   os_lock(device->operationLock);
   assert(0 == device->pendingOperation.attributeHandle);
   assert(PO_IDLE == device->pendingOperation.type);

   device->pendingOperation.attributeHandle   = attributeHandle;
   device->pendingOperation.type              = PO_READ;

   device->pendingRead.attributeValue    = attributeValue;
   device->pendingRead.attributeLength   = 0;
   device->pendingRead.attributeCapacity = attributeCapacity;

   enum HCI_StatusCode status = HCI_HOST_TIMEOUT;

   os_resetCondition(device->operationComplete);

   enum LB_STATUS commandStatus = controller->vendorFunctions->requestCharValue(device, attributeHandle);
   if (LB_OK != commandStatus)
   {
      goto done;
   }

   void* operationStatus = NULL;
   bool signaled = os_waitForCondition(device->operationComplete, 1000, &operationStatus);
   if (! signaled)
   {
      goto done;
   }

   *attributeLength = device->pendingRead.attributeLength;

   if (NULL == operationStatus)
   {
      status = HCI_STATUS_SUCCESS;
   }

done:

   device->pendingOperation.attributeHandle = 0;
   device->pendingOperation.type            = PO_IDLE;

   os_unlock(device->operationLock);

   if (HCI_STATUS_SUCCESS == status)
   {
      return LB_OK;
   }
   else
   {
      return LB_FAILURE;
   }
}

