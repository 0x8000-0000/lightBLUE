/**
 * @file lb_priv.h
 * @brief lightBLUE private implementation details
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

#ifndef __LB_PRIV_H__
#define __LB_PRIV_H__

/**
 * @privatesection
 */

#include <assert.h>

#include <osal_io.h>

#include <commands.h>

#define INVALID_CONNECTION_HANDLE  0xffff

struct lb_vendorFunctions
{
   void           (* on_vendorSpecificEvent)(struct LB_Controller* controller, const uint8_t* event, uint8_t length);
   void           (* on_metaEvent)(struct LB_Controller* controller, const uint8_t* event, uint8_t length);

   enum LB_STATUS (* initializeHCI)(struct LB_Controller* controller);
   enum LB_STATUS (* configureAsCentral)(struct LB_Controller* controller);

   enum LB_STATUS (* startDeviceDiscovery)(struct LB_Controller* controller);
   enum LB_STATUS (* stopDeviceDiscovery)(struct LB_Controller* controller);

   enum LB_STATUS (* openDeviceConnection)(struct LB_Controller* controller, const uint8_t* address);
   enum LB_STATUS (* closeDeviceConnection)(struct LB_Device* device);

   enum LB_STATUS (* startServiceDiscovery)(struct LB_Device* device);

   enum LB_STATUS (* writeCharValue)  (struct LB_Device* device, uint16_t attributeHandle, const uint8_t* attributeValue, uint8_t attributeLength);
   enum LB_STATUS (* requestCharValue)(struct LB_Device* device, uint16_t attributeHandle);
};

enum PendingOperation
{
   PO_IDLE,
   PO_DISCOVER,
   PO_READ,
   PO_WRITE,
};

struct LB_Controller;

struct LB_Device
{
   struct LB_Controller*   controller;

   uint16_t                connectionHandle;

   /*
    * The ATT_ReadResponse structure does not contain any attribute handle
    * so this means there can be only one in-flight read operation per
    * controller handle.
    *
    * Also, ST BlueNRG does not return ATT_WriteRsp as distinct from
    * ATT_ReadRsp, instead it returns GATT_PROC_COMPLETE, forcing us to
    * remember what command was in-flight.
    */

   struct os_lock*         operationLock;
   struct os_condition*    operationComplete;

   struct
   {
      uint16_t             type;                // enum PendingOperation above
      uint16_t             attributeHandle;
   }  pendingOperation;

   struct
   {
      uint8_t*             attributeValue;
      uint8_t              attributeCapacity;
      uint8_t              attributeLength;
   }  pendingRead;
};

struct LB_Controller
{
   struct io_channel* channel;

   // accumulator
   uint8_t  buffer[128];
   uint32_t length;

   struct os_lock*         operationLock;
   struct os_condition*    operationComplete;

   struct LB_Device  device[8];        // max Bluetooth device support

   const struct lb_vendorFunctions* vendorFunctions;

   uint16_t manufacturerId;
};

static inline struct LB_Device* getDevice(struct LB_Controller* controller, uint16_t connectionHandle)
{
   struct LB_Device* device = NULL;

   uint32_t ii = 0;
   while (ii < (sizeof(controller->device) / sizeof(controller->device[0])))
   {
      if (controller->device[ii].connectionHandle == connectionHandle)
      {
         device = &controller->device[ii];
         break;
      }
   }

   assert(device);
   return device;
}

static inline bool isDeviceConnected(struct LB_Device* device)
{
   return (INVALID_CONNECTION_HANDLE != device->connectionHandle);
}

void on_connectedToDevice(struct LB_Controller* controller, const uint8_t* address, uint16_t connectionHandle);

void on_disconnectedFromDevice(struct LB_Controller* controller, uint16_t connectionHandle, uint8_t reason);

void on_serviceDiscoveryComplete(struct LB_Controller* controller, uint16_t connectionHandle);


#endif // __LB_PRIV_H__

