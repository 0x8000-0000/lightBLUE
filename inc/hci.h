/**
 * @file hci.h
 * @brief HCI definitions
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

/**
 * @privatesection
 */

#ifndef __HCI_H__
#define __HCI_H__

#include <stdint.h>

struct LB_Controller;

enum HCI_PacketType
{
   HCI_PACKET_COMMAND          = 1,
   HCI_PACKET_ACL_DATA         = 2,
   HCI_PACKET_SYNCHRONOUS_DATA = 3,
   HCI_PACKET_EVENT            = 4,
};

struct BigEndianUnsigned16
{
   uint8_t lo;
   uint8_t hi;
};

struct HCI_CommandHeader
{
   uint8_t                    packetType;       // == HCI_PACKET_COMMAND
   struct BigEndianUnsigned16 opcode;
   uint8_t                    length;
};

enum HCI_EventOpcode
{
   HCI_EVENTID_Inquiry_Complete                         = 0x01,
   HCI_EVENTID_Inquiry_Result                           = 0x02,
   HCI_EVENTID_Connection_Complete                      = 0x03,
   HCI_EVENTID_Connection_Request                       = 0x04,
   HCI_EVENTID_Disconnection_Complete                   = 0x05,
   HCI_EVENTID_Authentication_Complete                  = 0x06,
   HCI_EVENTID_Remote_Name_Request_Complete             = 0x07,
   HCI_EVENTID_Encryption_Change                        = 0x08,
   HCI_EVENTID_Change_Connection_Link_Key_Complete      = 0x09,
   HCI_EVENTID_Master_Link_Key_Complete                 = 0x0A,
   HCI_EVENTID_Read_Remote_Supported_Features_Complete  = 0x0B,
   HCI_EVENTID_Read_Remote_Version_Information_Complete = 0x0C,
   HCI_EVENTID_QoS_Setup_Complete                       = 0x0D,
   HCI_EVENTID_Command_Complete                         = 0x0E,
   HCI_EVENTID_Command_Status                           = 0x0F,
   HCI_EVENTID_Hardware_Error                           = 0x10,
   HCI_EVENTID_Flush_Occurred                           = 0x11,
   HCI_EVENTID_Role_Change                              = 0x12,
   HCI_EVENTID_Number_Of_Completed_Packets              = 0x13,
   HCI_EVENTID_Mode_Change                              = 0x14,
   HCI_EVENTID_Return_Link_Keys                         = 0x15,
   HCI_EVENTID_PIN_Code_Request                         = 0x16,
   HCI_EVENTID_Link_Key_Request                         = 0x17,
   HCI_EVENTID_Link_Key_Notification                    = 0x18,
   HCI_EVENTID_Loopback_Command                         = 0x19,
   HCI_EVENTID_Data_Buffer_Overflow                     = 0x1A,
   HCI_EVENTID_Max_Slots_Change                         = 0x1B,
   HCI_EVENTID_Read_Clock_Offset_Complete               = 0x1C,
   HCI_EVENTID_Connection_Packet_Type_Changed           = 0x1D,
   HCI_EVENTID_QoS_Violation                            = 0x1E,
   HCI_EVENTID_Page_Scan_Mode_Change                    = 0x1F,
   HCI_EVENTID_Page_Scan_Repetition_Mode_Change         = 0x20,
   HCI_EVENTID_Flow_Specification_Complete              = 0x21,
   HCI_EVENTID_Inquiry_Result_with_RSSI                 = 0x22,
   HCI_EVENTID_Read_Remote_Extended_Features_Complete   = 0x23,

   HCI_EVENTID_Synchronous_Connection_Complete          = 0x2C,
   HCI_EVENTID_Synchronous_Connection_Changed           = 0x2D,
   HCI_EVENTID_Sniff_Subrate_Changed                    = 0x2E,
   HCI_EVENTID_Extended_Inquiry_Result                  = 0x2F,
   HCI_EVENTID_Refresh_Encryption_Key                   = 0x30,
   HCI_EVENTID_IO_Capability_Request                    = 0x31,
   HCI_EVENTID_IO_Capability_Response                   = 0x32,
   HCI_EVENTID_User_Confirmation_Request                = 0x33,
   HCI_EVENTID_User_Passkey_Request                     = 0x34,
   HCI_EVENTID_Remote_Oob_request                       = 0x35,
   HCI_EVENTID_Simple_Pairing_complete                  = 0x36,
   HCI_EVENTID_Remote_Oob_Response                      = 0x37,
   HCI_EVENTID_Link_Supervision_Timeout_Change          = 0x38,
   HCI_EVENTID_Enhanced_Flush_Occured                   = 0x39,
   HCI_EVENTID_Sniff_Request                            = 0x3A,

   HCI_EVENTID_Meta                                     = 0x3E,

   HCI_EVENTID_LE_Advertising_Packet_Report             = 0x42,

   HCI_EVENTID_LE_Remote_LL_Connection_Request          = 0x44,
   HCI_EVENTID_LE_LL_Connection_Created                 = 0x45,
   HCI_EVENTID_LE_LL_Connection_Termination             = 0x46,

   HCI_EVENTID_LE_Number_of_Completed_Packets           = 0x49,

   HCI_EVENTID_LE_Encryption_Requested                  = 0x4B,
   HCI_EVENTID_LE_Encrypted                             = 0x4C,

   HCI_EVENTID_LE_Ext_Gap                               = 0xE0,

   HCI_EVENTID_FM                                       = 0xF0,
   HCI_EVENTID_GPS_Channel_9                            = 0xF1,
   HCI_EVENTID_LE_Received_Data                         = 0xF2,
   HCI_EVENTID_BLE                                      = 0xFD,
   HCI_EVENTID_Bluetooth_Logo_Testing                   = 0xFE,
   HCI_EVENTID_Vendor_Specific                          = 0xFF,
};

enum HCI_Meta_Events
{
   HCI_LE_CONNECTION_COMPLETE_EVENT          = 0x01,
   HCI_LE_ADVERTISING_REPORT_EVENT           = 0x02,
   HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT   = 0x03,
   HCI_LE_READ_REMOTE_USED_FEATURES_EVENT    = 0x04,
   HCI_LE_LONG_TERM_KEY_REQUEST_EVENT        = 0x05,
};

struct HCI_EventHeader
{
   uint8_t  packetType;       // == HCI_PACKET_EVENT
   uint8_t  opcode;
   uint8_t  length;
};

enum HCI_CommandOpcode
{
   HCI_READ_LOCAL_VERSION_INFORMATION     = 0x1001,

   HCI_RESET                              = 0x0C03,

};


/*
 * if the command can be executed immediately
 */
struct HCI_EVENT_Command_Complete
{
   uint8_t                    numberHCICommands;
   struct BigEndianUnsigned16 opcode;
   uint8_t                    status;
};

void hci_on_eventCommandComplete(struct LB_Controller* controller, const struct HCI_EVENT_Command_Complete* event, uint8_t length);

/*
 * status returned first, then complete event returned when the command has completed
 */
struct HCI_EVENT_Command_Status
{
   uint8_t                    status;
   uint8_t                    numberHCICommands;
   struct BigEndianUnsigned16 opcode;
};

void hci_on_eventCommandStatus(struct LB_Controller* controller, const struct HCI_EVENT_Command_Status* event, uint8_t length);

struct HCI_EVENT_Vendor_Specific
{
   struct BigEndianUnsigned16 eventCode;
   uint8_t reasonCode;
};

void hci_on_vendorSpecificEvent(struct LB_Controller* controller, struct HCI_EVENT_Vendor_Specific* event, uint8_t length);

struct HCI_RESPONSE_Read_Local_Version_Information
{
   uint8_t                    hciVersion;
   struct BigEndianUnsigned16 hciRevision;
   uint8_t                    linkManagerProtocolVersion;
   struct BigEndianUnsigned16 manufacturerId;
   struct BigEndianUnsigned16 linkManagerProtocolSubversion;
};

enum HCI_StatusCode
{
   HCI_STATUS_SUCCESS                                    = 0x00,

   /*
    * ST names
    */
   HCI_UNKNOWN_COMMAND                                   = 0x01,
   HCI_NO_CONNECTION                                     = 0x02,
   HCI_HARDWARE_FAILURE                                  = 0x03,
   HCI_PAGE_TIMEOUT                                      = 0x04,
   HCI_AUTHENTICATION_FAILURE                            = 0x05,
   HCI_PIN_OR_KEY_MISSING                                = 0x06,
   HCI_MEMORY_FULL                                       = 0x07,
   HCI_CONNECTION_TIMEOUT                                = 0x08,
   HCI_MAX_NUMBER_OF_CONNECTIONS                         = 0x09,
   HCI_MAX_NUMBER_OF_SCO_CONNECTIONS                     = 0x0A,
   HCI_ACL_CONNECTION_EXISTS                             = 0x0B,
   HCI_COMMAND_DISALLOWED                                = 0x0C,
   HCI_REJECTED_LIMITED_RESOURCES                        = 0x0D,
   HCI_REJECTED_SECURITY                                 = 0x0E,
   HCI_REJECTED_PERSONAL                                 = 0x0F,
   HCI_HOST_TIMEOUT                                      = 0x10,
   HCI_UNSUPPORTED_FEATURE                               = 0x11,
   HCI_INVALID_PARAMETERS                                = 0x12,
   HCI_OE_USER_ENDED_CONNECTION                          = 0x13,
   HCI_OE_LOW_RESOURCES                                  = 0x14,
   HCI_OE_POWER_OFF                                      = 0x15,
   HCI_CONNECTION_TERMINATED                             = 0x16,
   HCI_REPEATED_ATTEMPTS                                 = 0x17,
   HCI_PAIRING_NOT_ALLOWED                               = 0x18,
   HCI_UNKNOWN_LMP_PDU                                   = 0x19,
   HCI_UNSUPPORTED_REMOTE_FEATURE                        = 0x1A,
   HCI_SCO_OFFSET_REJECTED                               = 0x1B,
   HCI_SCO_INTERVAL_REJECTED                             = 0x1C,
   HCI_AIR_MODE_REJECTED                                 = 0x1D,
   HCI_INVALID_LMP_PARAMETERS                            = 0x1E,
   HCI_UNSPECIFIED_ERROR                                 = 0x1F,
   HCI_UNSUPPORTED_LMP_PARAMETER_VALUE                   = 0x20,
   HCI_ROLE_CHANGE_NOT_ALLOWED                           = 0x21,
   HCI_LMP_RESPONSE_TIMEOUT                              = 0x22,
   HCI_LMP_ERROR_TRANSACTION_COLLISION                   = 0x23,
   HCI_LMP_PDU_NOT_ALLOWED                               = 0x24,
   HCI_ENCRYPTION_MODE_NOT_ACCEPTED                      = 0x25,
   HCI_UNIT_LINK_KEY_USED                                = 0x26,
   HCI_QOS_NOT_SUPPORTED                                 = 0x27,
   HCI_INSTANT_PASSED                                    = 0x28,
   HCI_PAIRING_NOT_SUPPORTED                             = 0x29,
   HCI_TRANSACTION_COLLISION                             = 0x2A,
   HCI_QOS_UNACCEPTABLE_PARAMETER                        = 0x2C,
   HCI_QOS_REJECTED                                      = 0x2D,
   HCI_CLASSIFICATION_NOT_SUPPORTED                      = 0x2E,
   HCI_INSUFFICIENT_SECURITY                             = 0x2F,
   HCI_PARAMETER_OUT_OF_RANGE                            = 0x30,
   HCI_ROLE_SWITCH_PENDING                               = 0x32,
   HCI_SLOT_VIOLATION                                    = 0x34,
   HCI_ROLE_SWITCH_FAILED                                = 0x35,
   HCI_EIR_TOO_LARGE                                     = 0x36,
   HCI_SIMPLE_PAIRING_NOT_SUPPORTED                      = 0x37,
   HCI_HOST_BUSY_PAIRING                                 = 0x38,
   HCI_CONN_REJ_NO_CH_FOUND                              = 0x39,
   HCI_CONTROLLER_BUSY                                   = 0x3A,
   HCI_UNACCEPTABLE_CONN_INTERV                          = 0x3B,
   HCI_DIRECTED_ADV_TIMEOUT                              = 0x3C,
   HCI_CONN_TERM_MIC_FAIL                                = 0x3D,
   HCI_CONN_FAIL_TO_BE_ESTABL                            = 0x3E,
   HCI_MAC_CONN_FAILED                                   = 0x3F,

   /*
    * TI names
    */
   HCI_ERROR_CODE_UNKNOWN_HCI_CMD                        = 0x01,
   HCI_ERROR_CODE_UNKNOWN_CONN_ID                        = 0x02,
   HCI_ERROR_CODE_HW_FAILURE                             = 0x03,
   HCI_ERROR_CODE_PAGE_TIMEOUT                           = 0x04,
   HCI_ERROR_CODE_AUTH_FAILURE                           = 0x05,
   HCI_ERROR_CODE_PIN_KEY_MISSING                        = 0x06,
   HCI_ERROR_CODE_MEM_CAP_EXCEEDED                       = 0x07,
   HCI_ERROR_CODE_CONN_TIMEOUT                           = 0x08,
   HCI_ERROR_CODE_CONN_LIMIT_EXCEEDED                    = 0x09,
   HCI_ERROR_CODE_SYNCH_CONN_LIMIT_EXCEEDED              = 0x0A,
   HCI_ERROR_CODE_ACL_CONN_ALREADY_EXISTS                = 0x0B,
   HCI_ERROR_CODE_CMD_DISALLOWED                         = 0x0C,
   HCI_ERROR_CODE_CONN_REJ_LIMITED_RESOURCES             = 0x0D,
   HCI_ERROR_CODE_CONN_REJECTED_SECURITY_REASONS         = 0x0E,
   HCI_ERROR_CODE_CONN_REJECTED_UNACCEPTABLE_BDADDR      = 0x0F,
   HCI_ERROR_CODE_CONN_ACCEPT_TIMEOUT_EXCEEDED           = 0x10,
   HCI_ERROR_CODE_UNSUPPORTED_FEATURE_PARAM_VALUE        = 0x11,
   HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS                 = 0x12,
   HCI_ERROR_CODE_REMOTE_USER_TERM_CONN                  = 0x13,
   HCI_ERROR_CODE_REMOTE_DEVICE_TERM_CONN_LOW_RESOURCES  = 0x14,
   HCI_ERROR_CODE_REMOTE_DEVICE_TERM_CONN_POWER_OFF      = 0x15,
   HCI_ERROR_CODE_CONN_TERM_BY_LOCAL_HOST                = 0x16,
   HCI_ERROR_CODE_REPEATED_ATTEMPTS                      = 0x17,
   HCI_ERROR_CODE_PAIRING_NOT_ALLOWED                    = 0x18,
   HCI_ERROR_CODE_UNKNOWN_LMP_PDU                        = 0x19,
   HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE             = 0x1A,
   HCI_ERROR_CODE_SCO_OFFSET_REJ                         = 0x1B,
   HCI_ERROR_CODE_SCO_INTERVAL_REJ                       = 0x1C,
   HCI_ERROR_CODE_SCO_AIR_MODE_REJ                       = 0x1D,
   HCI_ERROR_CODE_INVALID_LMP_PARAMS                     = 0x1E,
   HCI_ERROR_CODE_UNSPECIFIED_ERROR                      = 0x1F,
   HCI_ERROR_CODE_UNSUPPORTED_LMP_PARAM_VAL              = 0x20,
   HCI_ERROR_CODE_ROLE_CHANGE_NOT_ALLOWED                = 0x21,
   HCI_ERROR_CODE_LMP_LL_RESP_TIMEOUT                    = 0x22,
   HCI_ERROR_CODE_LMP_ERR_TRANSACTION_COLLISION          = 0x23,
   HCI_ERROR_CODE_LMP_PDU_NOT_ALLOWED                    = 0x24,
   HCI_ERROR_CODE_ENCRYPT_MODE_NOT_ACCEPTABLE            = 0x25,
   HCI_ERROR_CODE_LINK_KEY_CAN_NOT_BE_CHANGED            = 0x26,
   HCI_ERROR_CODE_REQ_QOS_NOT_SUPPORTED                  = 0x27,
   HCI_ERROR_CODE_INSTANT_PASSED                         = 0x28,
   HCI_ERROR_CODE_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED    = 0x29,
   HCI_ERROR_CODE_DIFFERENT_TRANSACTION_COLLISION        = 0x2A,
   HCI_ERROR_CODE_RESERVED1                              = 0x2B,
   HCI_ERROR_CODE_QOS_UNACCEPTABLE_PARAM                 = 0x2C,
   HCI_ERROR_CODE_QOS_REJ                                = 0x2D,
   HCI_ERROR_CODE_CHAN_ASSESSMENT_NOT_SUPPORTED          = 0x2E,
   HCI_ERROR_CODE_INSUFFICIENT_SECURITY                  = 0x2F,
   HCI_ERROR_CODE_PARAM_OUT_OF_MANDATORY_RANGE           = 0x30,
   HCI_ERROR_CODE_RESERVED2                              = 0x31,
   HCI_ERROR_CODE_ROLE_SWITCH_PENDING                    = 0x32,
   HCI_ERROR_CODE_RESERVED3                              = 0x33,
   HCI_ERROR_CODE_RESERVED_SLOT_VIOLATION                = 0x34,
   HCI_ERROR_CODE_ROLE_SWITCH_FAILED                     = 0x35,
   HCI_ERROR_CODE_EXTENDED_INQUIRY_RESP_TOO_LARGE        = 0x36,
   HCI_ERROR_CODE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST   = 0x37,
   HCI_ERROR_CODE_HOST_BUSY_PAIRING                      = 0x38,
   HCI_ERROR_CODE_CONN_REJ_NO_SUITABLE_CHAN_FOUND        = 0x39,
   HCI_ERROR_CODE_CONTROLLER_BUSY                        = 0x3A,
   HCI_ERROR_CODE_UNACCEPTABLE_CONN_PARAMETERS           = 0x3B,
   HCI_ERROR_CODE_DIRECTED_ADV_TIMEOUT                   = 0x3C,
   HCI_ERROR_CODE_CONN_TERM_MIC_FAILURE                  = 0x3D,
   HCI_ERROR_CODE_CONN_FAILED_TO_ESTABLISH               = 0x3E,
   HCI_ERROR_CODE_MAC_CONN_FAILED                        = 0x3F,
   HCI_ERROR_CODE_COARSE_CLOCK_ADJUST_REJECTED           = 0x40,
};

static inline uint16_t uint16Value(const struct BigEndianUnsigned16* beu16)
{
   return (((uint16_t) (beu16->hi)) << 8) | (beu16->lo);
}

/** Defines a static byte array containing an HCI command
 *
 * @private
 *
 * @param X is the HCI command opcode
 */
#define MAKE_HCI_COMMAND(X) \
   static const struct HCI_CommandHeader CMD_##X = \
{ \
   .packetType = HCI_PACKET_COMMAND, \
   .opcode     = \
   { \
      .lo      = HCI_##X & 0xFF, \
      .hi      = HCI_##X >> 8 \
   }, \
   .length     = 0 \
}

enum AdvertisingType
{
   AT_UNDIRECTED_SCANNABLE_AND_CONNECTABLE  = 0,
   AT_DIRECTED_NON_SCANNABLE                = 1,
   AT_SCANNABLE_NON_CONNECTABLE             = 2,
   AT_NON_CONNECTABLE_NON_SCANNABLE         = 3,
   AT_SCAN_RESPONSE                         = 4,
};

extern const char* AdvertisingTypeText[];

/*
 * 7.7.65.1 LE Connection Complete Event
 */
struct Event_HCI_LE_CONNECTION_COMPLETE
{
   uint8_t                    status;
   struct BigEndianUnsigned16 connectionHandle;
   uint8_t                    role;
   uint8_t                    peerAddressType;
   uint8_t                    peerAddress[6];
   struct BigEndianUnsigned16 connectionInterval;
   struct BigEndianUnsigned16 connectionLatency;
   struct BigEndianUnsigned16 supervisionTimeout;
   uint8_t                    clockAccuracy;
};

/*
 * 7.7.5 Disconnection Complete Event
 */
struct Event_HCI_DISCONNECTION_COMPLETE
{
   uint8_t                    status;
   struct BigEndianUnsigned16 connectionHandle;
   uint8_t                    reason;
};

/*
 * 3.4.4.10 Read by Group Type Response
 */
struct Event_ATT_READ_BY_GROUP_TYPE_RESP
{
   struct BigEndianUnsigned16 connectionHandle;
   uint8_t eventDataLength;            // 1 * attributeDataLength * attributeCount
   uint8_t attributeDataLength;        // 0x06 or 0x14
};


#endif // __HCI_H__

