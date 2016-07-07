/**
 * @file st_aci.c
 * @brief ST BlueNRG Application Command Interface
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
 *    BlueNRG Bluetooth LE stack application command interface (ACI)
 *    DocID 026257 Rev 4 / January 2016
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <osal_core.h>

#include <hci.h>
#include <commands.h>
#include <utils.h>
#include <gap.h>

#include "hci_priv.h"
#include "lb_priv.h"

/*
 * ACI definitions
 */

enum ST_HCI_CommandOpcode
{
   ACI_HAL_WRITE_CONFIG_DATA              = 0xFC0C,
   ACI_HAL_SET_TX_POWER_LEVEL             = 0xFC0F,

   ACI_GAP_INIT                           = 0xFC8A,
   ACI_GAP_TERMINATE                      = 0xFC93,
   ACI_GAP_START_GENERAL_DISCOVERY_PROC   = 0xFC97,
   ACI_GAP_CREATE_CONNECTION              = 0xFC9C,
   ACI_GAP_TERMINATE_GAP_PROC             = 0xFC9D,

   ACI_GATT_INIT                          = 0xFD01,

   ACI_GATT_DISC_ALL_PRIMARY_SERVICES     = 0xFD12,
   ACI_GATT_READ_CHAR_VALUE               = 0xFD18,
   ACI_GATT_WRITE_CHAR_VALUE              = 0xFD1C,
};

enum ACI_PARAM_OFFSET
{
   ACI_DATA_MODE       = 0x2D,
};

enum ACI_DATA_MODE_VALUE
{
   ACI_DATA_MODE_ONE_CONNECTION_SMALL_DB   = 1,
   ACI_DATA_MODE_ONE_CONNECTION_LARGE_DB   = 2,
   ACI_DATA_MODE_EIGHT_CONNECTIONS         = 3,
   ACI_DATA_MODE_FOUR_CONNECTIONS_SCANNING = 4,
};

enum ST_ACI_GAP_ROLE
{
   GAP_PERIPHERAL_ROLE  = 0x01,
   GAP_BROADCASTER_ROLE = 0x02,
   GAP_CENTRAL_ROLE     = 0x03,
   GAP_OBSERVER_ROLE    = 0x04,
};

enum ST_ACI_GAP_EVENTS
{
   ACI_BLUE_INITIALIZED_EVT              = 0x0001,
   ACI_GAP_DEVICE_FOUND_EVENT            = 0x0406,
   ACI_GAP_PROC_COMPLETE_EVENT           = 0x0407,

   EVT_BLUE_ATT_READ_RESP                = 0x0C07,
   EVT_BLUE_ATT_READ_BLOB_RESP           = 0x0C08,
   EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP  = 0x0C0A,
   EVT_BLUE_ATT_EXEC_WRITE_RESP          = 0x0C0D,
   EVT_BLUE_GATT_INDICATION              = 0x0C0E,
   EVT_BLUE_GATT_NOTIFICATION            = 0x0C0F,

   EVT_BLUE_GATT_PROCEDURE_COMPLETE      = 0x0C10,
   EVT_BLUE_GATT_ERROR_RESP              = 0x0C11,
};

enum ST_GAP_PROCEDURE_CODES
{
   GAP_LIMITED_DISCOVERY_PROC                  = 0x01,
   GAP_GENERAL_DISCOVERY_PROC                  = 0x02,
   GAP_NAME_DISCOVERY_PROC                     = 0x04,
   GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC      = 0x08,
   GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC   = 0x10,
   GAP_SELECTIVE_CONNECTION_ESTABLISHMENT_PROC = 0x20,
   GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC    = 0x40,
};

struct ACI_EVENT_GAP_Device_Found
{
   uint8_t     eventType;                 /**< Type of event (@ref ADV_IND, @ref ADV_DIRECT_IND, @ref ADV_SCAN_IND, @ref ADV_NONCONN_IND, @ref SCAN_RSP) */
   uint8_t     addressType;               /**< Type of the peer address (@ref PUBLIC_ADDR, @ref RANDOM_ADDR). */
   uint8_t     peerAddress[6];            /**< Address of the peer device found during scanning. */
   uint8_t     dataLength;                /**< Length of advertising or scan response data. */
   //uint8_t     dataRSSI[VARIABLE_SIZE];   /**< Advertising or scan response data + RSSI. RSSI is last octect (signed integer). */
};

struct ACI_EVENT_GAP_Procedure_Complete
{
  uint8_t code;                  /**< Terminated procedure. See @ref gap_procedure_codes "GAP procedure codes". */
  /**
   * @ref BLE_STATUS_SUCCESS, @ref BLE_STATUS_FAILED or @ref ERR_AUTH_FAILURE (procedure failed
   * due to authentication requirements).
   */
  uint8_t status;
  /**
   * Procedure specific data.\n
   * @li For Name Discovery Procedure:\n
   * the name of the peer device if the procedure completed successfully.
   * @li For General Connection Establishment Procedure:\n
   * The reconnection address written to the peripheral device if the peripheral is privacy enabled
   */
  //uint8_t data[VARIABLE_SIZE];
};

static const uint8_t CMD_ACI_SET_DATA_MODE[] =
{
   HCI_PACKET_COMMAND,
   ACI_HAL_WRITE_CONFIG_DATA & 0xFF,
   ACI_HAL_WRITE_CONFIG_DATA >> 8,
   3,                                     // length from here on
   ACI_DATA_MODE,
   1,                                     // length of following value
   ACI_DATA_MODE_ONE_CONNECTION_LARGE_DB,
};

static const uint8_t CMD_ACI_GATT_INIT[] =
{
   HCI_PACKET_COMMAND,
   ACI_GATT_INIT & 0xFF,
   ACI_GATT_INIT >> 8,
   0,
};

static enum LB_STATUS lb_performVendorSpecificInitialization_ST(struct LB_Controller* controller)
{
   enum LB_STATUS status;

   status = lb_resetHCI(controller);

   if (LB_OK == status)
   {
      status = lb_executeCommand(controller, CMD_ACI_SET_DATA_MODE, sizeof(CMD_ACI_SET_DATA_MODE), NULL, 0);
   }

   if (LB_OK == status)
   {
      status = lb_executeCommand(controller, CMD_ACI_GATT_INIT, sizeof(CMD_ACI_GATT_INIT), NULL, 0);
   }

   return status;
}

static const uint8_t ACI_SET_POWER_LEVEL[] =
{
   HCI_PACKET_COMMAND,
   ACI_HAL_SET_TX_POWER_LEVEL & 0xFF,
   ACI_HAL_SET_TX_POWER_LEVEL >> 8,
   2,                                     // length from here on
   1,                                     // high power on
   7,                                     // pa level (8dbm with high power on)
};

#if 0
static enum LB_STATUS lb_setPowerLevel_ST(struct LB_Controller* controller)
{
   return lb_executeCommand(controller, ACI_SET_POWER_LEVEL, sizeof(ACI_SET_POWER_LEVEL), NULL, 0);
}
#endif

static const uint8_t ACI_SET_ROLE_CENTRAL[] =
{
   HCI_PACKET_COMMAND,
   ACI_GAP_INIT & 0xFF,
   ACI_GAP_INIT >> 8,
   1,                                     // length from here on
   GAP_CENTRAL_ROLE,
};

static enum LB_STATUS lb_configureAsCentral_ST(struct LB_Controller* controller)
{
   enum LB_STATUS status = lb_executeCommand(controller, ACI_SET_ROLE_CENTRAL, sizeof(ACI_SET_ROLE_CENTRAL), NULL, 0);
   return status;
}

#define DEFAULT_SCAN_WINDOW      2000      // 2000 * 0.625msec = 1.25 seconds
#define DEFAULT_SCAN_INTERVAL    2000      // 2000 * 0.625msec = 1.25 seconds

static const uint8_t ACI_START_DISCOVERY[] =
{
   HCI_PACKET_COMMAND,
   ACI_GAP_START_GENERAL_DISCOVERY_PROC & 0xFF,
   ACI_GAP_START_GENERAL_DISCOVERY_PROC >> 8,
   6,                                     // length from here on
   DEFAULT_SCAN_INTERVAL & 0xFF,
   DEFAULT_SCAN_INTERVAL >> 8,
   DEFAULT_SCAN_WINDOW & 0xFF,
   DEFAULT_SCAN_WINDOW >> 8,
   0x01,                                  // random address
   0x01,                                  // filter duplicates
};

static const uint8_t ACI_STOP_DISCOVERY[] =
{
   HCI_PACKET_COMMAND,
   ACI_GAP_TERMINATE_GAP_PROC & 0xFF,
   ACI_GAP_TERMINATE_GAP_PROC >> 8,
   1,                                     // length from here on
   GAP_GENERAL_DISCOVERY_PROC,
};

static enum LB_STATUS lb_startDeviceDiscovery_ST(struct LB_Controller* controller)
{
   enum LB_STATUS status = lb_executeCommand(controller, ACI_START_DISCOVERY, sizeof(ACI_START_DISCOVERY), NULL, 0);
   return status;
}

static enum LB_STATUS lb_stopDeviceDiscovery_ST(struct LB_Controller* controller)
{
   enum LB_STATUS status = lb_executeCommand(controller, ACI_STOP_DISCOVERY, sizeof(ACI_STOP_DISCOVERY), NULL, 0);
   return status;
}

static void lb_on_vendorSpecificEvent_ST(struct LB_Controller* controller, const uint8_t* event, uint8_t length)
{
   if (lbDebugLevel > 100)
   {
      printf("Decoding ST vendor event:");
      utl_printBuffer(event, length);
      putchar('\n');
   }

   uint16_t eventCode = event[0] | (((uint16_t) event[1]) << 8);

   switch (eventCode)
   {
      case ACI_BLUE_INITIALIZED_EVT:
         break;

      case ACI_GAP_DEVICE_FOUND_EVENT:
         {
            struct ACI_EVENT_GAP_Device_Found* device = (struct ACI_EVENT_GAP_Device_Found*) (event + 2);
            const uint8_t* advertisingData = event + 2 + sizeof(*device);
            uint8_t advertisingDataLength = device->dataLength;
            int8_t rssi = advertisingData[advertisingDataLength];

            assert(AT_SCAN_RESPONSE >= device->eventType);
            lb_on_observedDeviceAdvertisment(controller, device->peerAddress, rssi, advertisingData, advertisingDataLength);
         }
         break;

      case ACI_GAP_PROC_COMPLETE_EVENT:
         {
            struct ACI_EVENT_GAP_Procedure_Complete* procComplete = (struct ACI_EVENT_GAP_Procedure_Complete*) (event + 2);
            switch (procComplete->code)
            {
               case GAP_GENERAL_DISCOVERY_PROC:
                  lb_on_deviceDiscoveryComplete(controller);
                  break;

               case GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC:
                  if (lbDebugLevel > 100)
                  {
                     printf("Connection complete; status %u\n", procComplete->status);
                  }
                  break;

               default:
                  if (lbDebugLevel > 10000)
                  {
                     printf("GAP Procedure complete; code %u  status %u\n", procComplete->code, procComplete->status);
                  }
                  break;
            }
         }
         break;

      case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
         hci_on_ATT_READ_BY_GROUP_TYPE_RESP_EVENT(controller, event + 2, length - 2);
         break;

      case EVT_BLUE_GATT_NOTIFICATION:
         {
            uint16_t connectionHandle = event[2] | (((uint16_t) event[3]) << 8);
            struct LB_Device* device = getDevice(controller, connectionHandle);
            uint8_t attributeLength = event[4];
            uint16_t attributeHandle = event[5] | (((uint16_t) event[6]) << 8);

            lb_on_receivedNotification(device, attributeHandle, 0, &event[7], attributeLength);
         }
         break;

      case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
         {
            uint16_t connectionHandle = event[2] | (((uint16_t) event[3]) << 8);
            struct LB_Device* device = getDevice(controller, connectionHandle);

            if (PO_DISCOVER == device->pendingOperation.type)
            {
               on_serviceDiscoveryComplete(controller, connectionHandle);
            }
            else
            {
               assert((PO_READ == device->pendingOperation.type) || (PO_WRITE == device->pendingOperation.type));
               assert(device->pendingOperation.attributeHandle);

               os_signalCondition(device->operationComplete, (void*) (uintptr_t) event[5]);
            }
         }
         break;

      case EVT_BLUE_ATT_READ_RESP:
         {
            uint16_t connectionHandle = event[2] | (((uint16_t) event[3]) << 8);
            struct LB_Device* device = getDevice(controller, connectionHandle);

            assert(device->pendingOperation.attributeHandle);
            assert(PO_READ == device->pendingOperation.type);

            uint8_t attributeLength = event[4];
            //printf("Length: %u   AttributeLength: %u\n", length, attributeLength);
            assert((attributeLength + 5) == length);

            if (attributeLength < device->pendingRead.attributeCapacity)
            {
               attributeLength = device->pendingRead.attributeCapacity;
            }
            memcpy(device->pendingRead.attributeValue, &event[5], attributeLength);
            device->pendingRead.attributeLength = attributeLength;
         }
         break;

      case EVT_BLUE_GATT_ERROR_RESP:
         // ignore for now
         break;

      default:
         printf("ST Vendor specific event: (code: %04x) -- ", (unsigned) eventCode);
         utl_printBuffer(event, length);
         putchar('\n');
         fflush(stdout);
         break;
   }
}

static void lb_on_metaEvent_ST(struct LB_Controller* controller, const uint8_t* event, uint8_t length)
{
   assert(length);
   switch (event[0])    // subevent code
   {
      case HCI_LE_CONNECTION_COMPLETE_EVENT:
         {
            assert(sizeof(struct Event_HCI_LE_CONNECTION_COMPLETE) + 1 == length);
            const struct Event_HCI_LE_CONNECTION_COMPLETE* linkEvent = (const struct Event_HCI_LE_CONNECTION_COMPLETE*) (event + 1);
            on_connectedToDevice(controller, linkEvent->peerAddress, uint16Value(&linkEvent->connectionHandle));
         }
         break;
      case HCI_LE_ADVERTISING_REPORT_EVENT:
         break;
      case HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT:
         break;
      case HCI_LE_READ_REMOTE_USED_FEATURES_EVENT:
         break;
      case HCI_LE_LONG_TERM_KEY_REQUEST_EVENT:
         break;
      default:
         break;
   }
}

static const uint8_t ACI_OPEN_CONNECTION_CMD[] =
{
   HCI_PACKET_COMMAND,
   ACI_GAP_CREATE_CONNECTION & 0xFF,
   ACI_GAP_CREATE_CONNECTION >> 8,
   0x18,                            // length of command
   0xD0,0x07,                       // scan interval
   0xD0,0x07,                       // scan window
   0x00,                            // peer address type: public
   0x00,0x00,0x00,0x00,0x00,0x00,   // peer address
   0x00,                            // own address type: public
   0x14,0x00,                       // controller interval min
   0x28,0x00,                       // controller interval max
   0x00,0x00,                       // controller latency
   0x64,0x00,                       // supervision timeout
   0x02,0x00,                       // minimum CE length
   0x02,0x00,                       // maximum CE length
};

static enum LB_STATUS lb_openDeviceConnection_ST(struct LB_Controller* controller, const uint8_t* address)
{
   uint8_t cmd[sizeof(ACI_OPEN_CONNECTION_CMD) + 6];
   memcpy(cmd, ACI_OPEN_CONNECTION_CMD, sizeof(ACI_OPEN_CONNECTION_CMD));
   memcpy(cmd + 9, address, 6);

   enum LB_STATUS status = lb_executeCommand(controller, cmd, sizeof(cmd), NULL, 0);
   return status;
}

static const uint8_t ACI_TERMINATE_CONNECTION_CMD[] =
{
   HCI_PACKET_COMMAND,
   ACI_GAP_TERMINATE & 0xFF,
   ACI_GAP_TERMINATE >> 8,
   3,                         // parameter length
   0,
   0,
   HCI_ERROR_CODE_REMOTE_USER_TERM_CONN,
};

static enum LB_STATUS lb_closeDeviceConnection_ST(struct LB_Device* device)
{
   uint8_t cmd[sizeof(ACI_TERMINATE_CONNECTION_CMD)];
   memcpy(cmd, ACI_TERMINATE_CONNECTION_CMD, sizeof(ACI_TERMINATE_CONNECTION_CMD));
   cmd[4] = device->connectionHandle & 0xFF;
   cmd[5] = device->connectionHandle >> 8;

   enum LB_STATUS status = lb_executeCommand(device->controller, cmd, sizeof(cmd), NULL, 0);
   return status;
}

static enum LB_STATUS lb_startServiceDiscovery_ST(struct LB_Device* device)
{
   uint8_t cmd[] =
   {
      HCI_PACKET_COMMAND,
      ACI_GATT_DISC_ALL_PRIMARY_SERVICES & 0xFF,
      ACI_GATT_DISC_ALL_PRIMARY_SERVICES >> 8,
      2,
      device->connectionHandle & 0xFF,
      device->connectionHandle >> 8,
   };

   enum LB_STATUS status = lb_executeCommand(device->controller, cmd, sizeof(cmd), NULL, 0);
   return status;
}

static enum LB_STATUS lb_writeCharValue_ST(struct LB_Device* device, uint16_t attributeHandle, const uint8_t* attributeValue, uint8_t attributeLength)
{
   if (lbDebugLevel > 1000)
   {
      printf("-> ST Write Char value for handle %04x: ", attributeHandle);
      utl_printBuffer(attributeValue, attributeLength);
      putchar('\n');
   }

   uint8_t cmd[64];
   cmd[0] = HCI_PACKET_COMMAND;
   cmd[1] = ACI_GATT_WRITE_CHAR_VALUE & 0xFF;
   cmd[2] = ACI_GATT_WRITE_CHAR_VALUE >> 8;
   cmd[3] = 5 + attributeLength;
   cmd[4] = device->connectionHandle & 0xFF;
   cmd[5] = device->connectionHandle >> 8;
   cmd[6] = attributeHandle & 0xFF;
   cmd[7] = attributeHandle >> 8;
   cmd[8] = attributeLength;
   memcpy(&cmd[9], attributeValue, attributeLength);

   enum LB_STATUS status = lb_executeCommand(device->controller, cmd, 9 + attributeLength, NULL, 0);
   return status;
}

static enum LB_STATUS lb_requestCharValue_ST(struct LB_Device* device, uint16_t attributeHandle)
{
   if (lbDebugLevel > 1000)
   {
      printf("-> ST Request Char value for handle %04x\n", attributeHandle);
   }

   uint8_t cmd[] =
   {
      HCI_PACKET_COMMAND,
      ACI_GATT_READ_CHAR_VALUE & 0xFF,
      ACI_GATT_READ_CHAR_VALUE >> 8,
      4,
      device->connectionHandle & 0xFF,
      device->connectionHandle >> 8,
      attributeHandle & 0xFF,
      attributeHandle >> 8,
   };

   enum LB_STATUS status = lb_executeCommand(device->controller, cmd, sizeof(cmd), NULL, 0);
   return status;
}

struct lb_vendorFunctions lb_vendorFunctions_ST =
{
   .on_vendorSpecificEvent  = lb_on_vendorSpecificEvent_ST,
   .on_metaEvent            = lb_on_metaEvent_ST,
   .initializeHCI           = lb_performVendorSpecificInitialization_ST,
   .configureAsCentral      = lb_configureAsCentral_ST,

   .startDeviceDiscovery    = lb_startDeviceDiscovery_ST,
   .stopDeviceDiscovery     = lb_stopDeviceDiscovery_ST,

   .openDeviceConnection    = lb_openDeviceConnection_ST,
   .closeDeviceConnection   = lb_closeDeviceConnection_ST,

   .startServiceDiscovery   = lb_startServiceDiscovery_ST,

   .writeCharValue          = lb_writeCharValue_ST,
   .requestCharValue        = lb_requestCharValue_ST,

};


