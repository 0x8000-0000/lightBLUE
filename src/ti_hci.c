/**
 * @file ti_hci.c
 * @brief TI BLE Host Control Interface
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
 * Based on
 *    TI BLE Vendor Specific HCI Reference Guide Version 2.1.0
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <osal_core.h>

#include <hci.h>
#include <commands.h>
#include <utils.h>
#include <gap.h>

#include "lb_priv.h"
#include "hci_priv.h"

enum TI_HCI_CommandOpcode
{
   HCI_EXT_GAP_DEVICE_INIT                = 0xFE00,
   HCI_EXT_GAP_DEVICE_DISC_REQ            = 0xFE04,
   HCI_EXT_GAP_DEVICE_DISC_CANCEL         = 0xFE05,
   HCI_EXT_GAP_EST_LINK_REQ               = 0xFE09,
   GAP_TerminateLinkReq                   = 0xFE0A,

   GATT_ReadCharValue                     = 0xFD8A,
   GATT_DiscAllPrimaryServices            = 0xFD90,
   GATT_WriteCharValue                    = 0xFD92,

};

/*
 * TI-specific HCI definitions
 */
enum TI_HCI_GAP_EVENTS
{
   GAP_DeviceInitDone          = 0x0600,
   GAP_DeviceDiscovery         = 0x0601,
   GAP_LinkEstablished         = 0x0605,
   GAP_LinkTerminated          = 0x0606,
   GAP_DeviceInformation       = 0x060D,
   CommandStatus               = 0x067F,

   ATT_ErrorRsp                = 0x0501,
   ATT_ReadRsp                 = 0x050B,
   ATT_ReadByGrpTypeRsp        = 0x0511,
   ATT_WriteRsp                = 0x0513,
   ATT_HandleValueNotification = 0x051B,
};

enum StatusTypes
{
   BLE_SUCCESS                                     = 0x00,
   BLE_FAILURE                                     = 0x01,
   INVALIDPARAMETER                                = 0x02,
   INVALID_TASK                                    = 0x03,
   MSG_BUFFER_NOT_AVAIL                            = 0x04,
   INVALID_MSG_POINTER                             = 0x05,
   INVALID_EVENT_ID                                = 0x06,
   INVALID_INTERRUPT_ID                            = 0x07,
   NO_TIMER_AVAIL                                  = 0x08,
   NV_ITEM_UNINIT                                  = 0x09,
   NV_OPER_FAILED                                  = 0x0A,
   INVALID_MEM_SIZE                                = 0x0B,
   NV_BAD_ITEM_LEN                                 = 0x0C,
   bleNotReady                                     = 0x10,
   bleAlreadyInRequestedMode                       = 0x11,
   bleIncorrectMode                                = 0x12,
   bleMemAllocError                                = 0x13,
   bleNotConnected                                 = 0x14,
   bleNoResources                                  = 0x15,
   blePending                                      = 0x16,
   bleTimeout                                      = 0x17,
   bleInvalidRange                                 = 0x18,
   bleLinkEncrypted                                = 0x19,
   bleProcedureComplete                            = 0x1A,
   bleUnexpectedRole                               = 0x21,
   bleGAPUserCanceled                              = 0x30,
   bleGAPConnNotAcceptable                         = 0x31,
   bleGAPBondRejected                              = 0x32,
   bleInvalidPDU                                   = 0x40,
   bleInsufficientAuthen                           = 0x41,
   bleInsufficientEncrypt                          = 0x42,
   bleInsufficientKeySize                          = 0x43,
};

static enum LB_STATUS lb_performVendorSpecificInitialization_TI(struct LB_Controller* controller)
{
   return LB_OK;
}

#if 0
static enum LB_STATUS lb_setPowerLevel_TI(struct LB_Controller* controller)
{
   return LB_OK;
}
#endif

static const uint8_t TI_GAP_Device_Init[] =
{
   HCI_PACKET_COMMAND,
   HCI_EXT_GAP_DEVICE_INIT & 0xFF,
   HCI_EXT_GAP_DEVICE_INIT >> 8,
   0x26,                            // length of command
   0x08,                            // central
   0x05,                            // max scan response
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // IRK
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    // CSRK
   0x01, 0x00, 0x00, 0x00           // sign counter
};

static enum LB_STATUS lb_configureAsCentral_TI(struct LB_Controller* controller)
{
   enum LB_STATUS status = lb_executeCommand(controller, TI_GAP_Device_Init, sizeof(TI_GAP_Device_Init), NULL, 0);
   return status;
}

static const uint8_t TI_START_DISCOVERY_CMD[] =
{
   HCI_PACKET_COMMAND,
   HCI_EXT_GAP_DEVICE_DISC_REQ & 0xFF,
   HCI_EXT_GAP_DEVICE_DISC_REQ >> 8,
   3,                                        // parameter length
   0x03,          // all
   1,             // active scan?
   0,             // whitelist only?
};

static enum LB_STATUS lb_startDeviceDiscovery_TI(struct LB_Controller* controller)
{
   enum LB_STATUS status = lb_executeCommand(controller, TI_START_DISCOVERY_CMD, sizeof(TI_START_DISCOVERY_CMD), NULL, 0);
   return status;
}

static const uint8_t TI_STOP_DISCOVERY_CMD[] =
{
   HCI_PACKET_COMMAND,
   HCI_EXT_GAP_DEVICE_DISC_CANCEL & 0xFF,
   HCI_EXT_GAP_DEVICE_DISC_CANCEL >> 8,
   0,                                     // parameter length
};

static enum LB_STATUS lb_stopDeviceDiscovery_TI(struct LB_Controller* controller)
{
   enum LB_STATUS status = lb_executeCommand(controller, TI_STOP_DISCOVERY_CMD, sizeof(TI_STOP_DISCOVERY_CMD), NULL, 0);
   return status;
}

struct Event_GAP_DeviceInitDone
{
   uint8_t      status;
   uint8_t      myAddr[6];
   struct BigEndianUnsigned16 dataPktLen;
   uint8_t      numDataPkts;
   uint8_t      IRK[16];
   uint8_t      CSRK[16];
};

struct Event_GAP_DeviceInformation
{
   uint8_t      status;
   uint8_t      eventType;
   uint8_t      addrType;
   uint8_t      addr[6];
   int8_t       rssi;
   uint8_t      dataLength;
   //uint8_t      dataField[];
};

struct Event_GAP_DeviceDiscoveryDone
{
   uint8_t      status;
   uint8_t      deviceCount;
   // struct Event_GAP_DeviceDiscoveryElement device[];
};

struct Event_GAP_DeviceDiscoveryElement
{
   uint8_t     eventType;
   uint8_t     addrType;
   uint8_t     addr[6];
};

struct Event_GAP_CommandStatus
{
   uint8_t                    status;
   struct BigEndianUnsigned16 opcode;
   uint8_t                    dataLength;
};

/*
 * TI's version of Event_HCI_LE_CONNECTION_COMPLETE. Slightly different, because.
 */
struct Event_GAP_LinkEstablished
{
   uint8_t                    status;
   uint8_t                    peerAddressType;
   uint8_t                    peerAddress[6];
   struct BigEndianUnsigned16 connectionHandle;
   uint8_t                    myRole;
   struct BigEndianUnsigned16 connectionInterval;
   struct BigEndianUnsigned16 connectionLatency;
   struct BigEndianUnsigned16 connectionTimeout;
   uint8_t                    clockAccuracy;
};

struct Event_GAP_LinkTerminated
{
   uint8_t                    status;
   struct BigEndianUnsigned16 connectionHandle;
   uint8_t                    reason;
};

static void lb_on_vendorSpecificEvent_TI(struct LB_Controller* controller, const uint8_t* event, uint8_t length)
{
   if (lbDebugLevel > 100)
   {
      printf("Decoding TI vendor event:");
      utl_printBuffer(event, length);
      putchar('\n');
   }

   uint16_t eventCode = event[0] | (((uint16_t) event[1]) << 8);

   switch (eventCode)
   {
      case GAP_DeviceInitDone:
         {
            assert((sizeof(struct Event_GAP_DeviceInitDone) + 2) == length);
            const struct Event_GAP_DeviceInitDone* initDone = (const struct Event_GAP_DeviceInitDone*) (event + 2);
            if (lbDebugLevel > 10000)
            {
               printf("TI: Device initialization done. Status %u. Local address: ", initDone->status);
               utl_printAddress(initDone->myAddr);
               putchar('\n');
            }
         }
         break;

      case GAP_LinkEstablished:
         {
            assert((sizeof(struct Event_GAP_LinkEstablished) + 2) == length);
            const struct Event_GAP_LinkEstablished* linkEvent = (const struct Event_GAP_LinkEstablished*) (event + 2);
            on_connectedToDevice(controller, linkEvent->peerAddress, uint16Value(&linkEvent->connectionHandle));
         }
         break;

      case GAP_LinkTerminated:
         {
            assert((sizeof(struct Event_HCI_DISCONNECTION_COMPLETE) + 2) == length);
            const struct Event_HCI_DISCONNECTION_COMPLETE* linkEvent = (const struct Event_HCI_DISCONNECTION_COMPLETE*) (event + 2);
            on_disconnectedFromDevice(controller, uint16Value(&linkEvent->connectionHandle), linkEvent->reason);
         }
         break;

      case GAP_DeviceInformation:
         {
            assert((sizeof(struct Event_GAP_DeviceInformation) + 2) <= length);
            const struct Event_GAP_DeviceInformation* deviceInfo = (const struct Event_GAP_DeviceInformation*) (event + 2);

            const uint8_t* advertisingData = event + 2 + sizeof(*deviceInfo);
            uint8_t advertisingDataLength = deviceInfo->dataLength;
            assert(AT_SCAN_RESPONSE >= deviceInfo->eventType);
            lb_on_observedDeviceAdvertisment(controller, deviceInfo->addr, deviceInfo->rssi, advertisingData, advertisingDataLength);
         }
         break;

      case GAP_DeviceDiscovery:
         {
            assert((sizeof(struct Event_GAP_DeviceDiscoveryDone) + 2) <= length);
            const struct Event_GAP_DeviceDiscoveryDone* discoveryDone = (const struct Event_GAP_DeviceDiscoveryDone*) (event + 2);
            if (lbDebugLevel > 10000)
            {
               printf("TI: Device discover complete. Status %u. Found %u devices.\n", discoveryDone->status, discoveryDone->deviceCount);
               assert((sizeof(struct Event_GAP_DeviceDiscoveryDone) + 2 + discoveryDone->deviceCount * sizeof(struct Event_GAP_DeviceDiscoveryElement)) == length);
               const struct Event_GAP_DeviceDiscoveryElement* deviceInfo = (const struct Event_GAP_DeviceDiscoveryElement*) (event + sizeof(struct Event_GAP_DeviceDiscoveryDone) + 2);
               for (uint8_t ii = 0; ii < discoveryDone->deviceCount; ii ++)
               {
                  putchar(' ');
                  putchar(' ');
                  putchar(' ');
                  utl_printAddress(deviceInfo[ii].addr);
                  putchar('\n');
               }
            }
            lb_on_deviceDiscoveryComplete(controller);
         }
         break;

      case CommandStatus:
         {
            assert((sizeof(struct Event_GAP_CommandStatus) + 2) <= length);
            const struct Event_GAP_CommandStatus* commandStatus = (const struct Event_GAP_CommandStatus*) (event + 2);
            const uint8_t* commandResult = event + 2 + sizeof(struct Event_GAP_CommandStatus);
            if (0 == commandStatus->dataLength)
            {
               commandResult = NULL;
            }
            signalCondition(uint16Value(&commandStatus->opcode), commandStatus->status, commandResult, commandStatus->dataLength);
         }
         break;

      case ATT_ReadByGrpTypeRsp:
         if (BLE_SUCCESS == event[2])
         {
            hci_on_ATT_READ_BY_GROUP_TYPE_RESP_EVENT(controller, event + 3, length - 3);
         }
         else if (bleProcedureComplete == event[2])
         {
            uint16_t connectionHandle = event[3] | (((uint16_t) event[4]) << 8);
            on_serviceDiscoveryComplete(controller, connectionHandle);
         }
         else
         {
            printf("Unexpected status for ATT_ReadByGrpTypeRsp: 0x%x\n", (unsigned) event[2]);
            assert(false);
         }
         break;

      case ATT_ErrorRsp:
         {
            uint16_t connectionHandle = event[3] | (((uint16_t) event[4]) << 8);
            struct LB_Device* device = getDevice(controller, connectionHandle);

            uint16_t attributeHandle = event[7] | (((uint16_t) event[8]) << 8);
            uint8_t status = event[9];

            if (lbDebugLevel > 100)
            {
               printf("TI ErrorRsp; connection: %04x, attribute: %04x, status: %02x\n", connectionHandle, attributeHandle, status);
            }

            if ((PO_READ == device->pendingOperation.type) || (PO_WRITE == device->pendingOperation.type))
            {
               assert(attributeHandle == device->pendingOperation.attributeHandle);
               os_signalCondition(device->operationComplete, (void*) (uintptr_t) status);
            }
         }
         break;

      case ATT_WriteRsp:
         {
            uint16_t connectionHandle = event[3] | (((uint16_t) event[4]) << 8);
            struct LB_Device* device = getDevice(controller, connectionHandle);

            assert(device->pendingOperation.attributeHandle);
            assert(PO_WRITE == device->pendingOperation.type);
            //printf("TI WriteResponse status: %02x\n", event[2]);
            os_signalCondition(device->operationComplete, (void*) (uintptr_t) event[2]);
         }
         break;

      case ATT_ReadRsp:
         {
            uint16_t connectionHandle = event[3] | (((uint16_t) event[4]) << 8);
            struct LB_Device* device = getDevice(controller, connectionHandle);

            assert(device->pendingOperation.attributeHandle);
            assert(PO_READ == device->pendingOperation.type);

            uint8_t attributeLength = event[5];
            //printf("Length: %u   AttributeLength: %u\n", length, attributeLength);
            assert((attributeLength + 6) == length);
            if (attributeLength < device->pendingRead.attributeCapacity)
            {
               attributeLength = device->pendingRead.attributeCapacity;
            }
            memcpy(device->pendingRead.attributeValue, &event[6], attributeLength);
            device->pendingRead.attributeLength = attributeLength;

            os_signalCondition(device->operationComplete, (void*) (uintptr_t) event[2]);
         }
         break;

      case ATT_HandleValueNotification:
         {
            uint8_t status = event[2];
            uint16_t connectionHandle = event[3] | (((uint16_t) event[4]) << 8);
            struct LB_Device* device = getDevice(controller, connectionHandle);
            uint8_t attributeLength = event[5];
            uint16_t attributeHandle = event[6] | (((uint16_t) event[7]) << 8);

            lb_on_receivedNotification(device, attributeHandle, status, &event[8], attributeLength);
         }
         break;

      default:
         printf("TI Vendor specific event: (code: %04x) -- ", (unsigned) eventCode);
         utl_printBuffer(event, length);
         putchar('\n');
         fflush(stdout);
         break;
   }
}

static const uint8_t TI_OPEN_CONNECTION_CMD[] =
{
   HCI_PACKET_COMMAND,
   HCI_EXT_GAP_EST_LINK_REQ & 0xFF,
   HCI_EXT_GAP_EST_LINK_REQ >> 8,
   9,                                     // parameter length
   0,                                     // high-duty cycle: false
   0,                                     // white list: false
   0,                                     // address type peer: public
};

static enum LB_STATUS lb_openDeviceConnection_TI(struct LB_Controller* controller, const uint8_t* address)
{
   uint8_t cmd[sizeof(TI_OPEN_CONNECTION_CMD) + 6];
   memcpy(cmd, TI_OPEN_CONNECTION_CMD, sizeof(TI_OPEN_CONNECTION_CMD));
   memcpy(cmd + sizeof(TI_OPEN_CONNECTION_CMD), address, 6);

   enum LB_STATUS status = lb_executeCommand(controller, cmd, sizeof(cmd), NULL, 0);
   return status;
}

static enum LB_STATUS lb_closeDeviceConnection_TI(struct LB_Device* device)
{
   uint8_t cmd[] =
   {
      HCI_PACKET_COMMAND,
      GAP_TerminateLinkReq & 0xFF,
      GAP_TerminateLinkReq >> 8,
      3,                                     // parameter length
      device->connectionHandle & 0xFF,
      device->connectionHandle >> 8,
      HCI_ERROR_CODE_REMOTE_USER_TERM_CONN,
   };

   enum LB_STATUS status = lb_executeCommand(device->controller, cmd, sizeof(cmd), NULL, 0);
   return status;
}

static enum LB_STATUS lb_startServiceDiscovery_TI(struct LB_Device* device)
{
   uint8_t cmd[] =
   {
      HCI_PACKET_COMMAND,
      GATT_DiscAllPrimaryServices & 0xFF,
      GATT_DiscAllPrimaryServices >> 8,
      2,
      device->connectionHandle & 0xFF,
      device->connectionHandle >> 8,
   };

   enum LB_STATUS status = lb_executeCommand(device->controller, cmd, sizeof(cmd), NULL, 0);
   return status;
}

static enum LB_STATUS lb_writeCharValue_TI(struct LB_Device* device, uint16_t attributeHandle, const uint8_t* attributeValue, uint8_t attributeLength)
{
   if (lbDebugLevel > 1000)
   {
      printf("-> TI Write Char value for handle %04x: ", attributeHandle);
      utl_printBuffer(attributeValue, attributeLength);
      putchar('\n');
   }

   uint8_t cmd[64];
   cmd[0] = HCI_PACKET_COMMAND;
   cmd[1] = GATT_WriteCharValue & 0xFF;
   cmd[2] = GATT_WriteCharValue >> 8;
   cmd[3] = 4 + attributeLength;
   cmd[4] = device->connectionHandle & 0xFF;
   cmd[5] = device->connectionHandle >> 8;
   cmd[6] = attributeHandle & 0xFF;
   cmd[7] = attributeHandle >> 8;
   memcpy(&cmd[8], attributeValue, attributeLength);

#if 0
   printf("TI writeCharValue: ");
   utl_printBuffer(cmd, 8 + attributeLength);
   putchar('\n');
#endif

   enum LB_STATUS status = lb_executeCommand(device->controller, cmd, 8 + attributeLength, NULL, 0);
   return status;
}

static enum LB_STATUS lb_requestCharValue_TI(struct LB_Device* device, uint16_t attributeHandle)
{
   if (lbDebugLevel > 1000)
   {
      printf("-> TI Request Char value for handle %04x\n", attributeHandle);
   }

   uint8_t cmd[] =
   {
      HCI_PACKET_COMMAND,
      GATT_ReadCharValue & 0xFF,
      GATT_ReadCharValue >> 8,
      4,
      device->connectionHandle & 0xFF,
      device->connectionHandle >> 8,
      attributeHandle & 0xFF,
      attributeHandle >> 8,
   };

   enum LB_STATUS status = lb_executeCommand(device->controller, cmd, sizeof(cmd), NULL, 0);
   return status;
}

struct lb_vendorFunctions lb_vendorFunctions_TI =
{
   .on_vendorSpecificEvent  = lb_on_vendorSpecificEvent_TI,
   .on_metaEvent            = NULL,

   .initializeHCI           = lb_performVendorSpecificInitialization_TI,
   .configureAsCentral      = lb_configureAsCentral_TI,

   .startDeviceDiscovery    = lb_startDeviceDiscovery_TI,
   .stopDeviceDiscovery     = lb_stopDeviceDiscovery_TI,

   .openDeviceConnection    = lb_openDeviceConnection_TI,
   .closeDeviceConnection   = lb_closeDeviceConnection_TI,

   .startServiceDiscovery   = lb_startServiceDiscovery_TI,

   .writeCharValue          = lb_writeCharValue_TI,
   .requestCharValue        = lb_requestCharValue_TI,
};

