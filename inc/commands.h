/**
 * @file commands.h
 * @brief HCI commands interface
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

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <hci.h>

/** @addtogroup lightBLUE lightBLUE
 *
 * @{
 *
 * @addtogroup lightBLUE_controller Controller Interface
 *
 * @{
 */

/** Opaque controller connection
 */
struct LB_Controller;

/** Opaque device connection
 */
struct LB_Device;

/** Status code returned from all lightBLUE methods
 */
enum LB_STATUS
{
   LB_OK,
   LB_FAILURE,
   LB_UNKNOWN_VENDOR,
   LB_CONTROLLER_NOT_CONNECTED,
   LB_DEVICE_NOT_CONNECTED,
   LB_OPERATION_TIMEOUT,
};

/** Sends a formatted command buffer to the controller
 *
 * An optional response buffer (and size) can be passed in, and will store
 * any data received from the controller in the command acknowledgment packet.
 *
 * @private
 *
 * @param controller is the Bluetooth controller
 * @param command is the actual command
 * @param commandLength is the length of the command
 * @param response will contain the response from the acknowledgment packet
 * @param maxResponseLength is the size of the response buffer
 * @return status
 */
enum LB_STATUS lb_executeCommand(struct LB_Controller* controller, const uint8_t* command, uint8_t commandLength, uint8_t* response, uint8_t maxResponseLength);

/** Requests the version of the Bluetooth controller hardware and firmware.
 *
 * @param controller is the Bluetooth controller
 * @param[out] version contains the Bluetooth version information
 * @return status
 */
enum LB_STATUS lb_readLocalVersionInformation(struct LB_Controller* controller, struct HCI_RESPONSE_Read_Local_Version_Information* version);

/** Requests the reset of the firmware on the Bluetooth controller
 *
 * @param controller is the Bluetooth controller
 * @return status
 */
enum LB_STATUS lb_resetHCI(struct LB_Controller* controller);

/** Enables selectively dumping to stdout of various debug information
 *
 * @param level is the debug level
 */
void lb_setDebugLevel(uint32_t level);

/** Request manufacturer-specific initialization of a Bluetooth controller
 *
 * @param controller is the Bluetooth controller
 * @return status
 */
enum LB_STATUS lb_initializeHCI(struct LB_Controller* controller);

/** Configures the Bluetooth controller as a 'central' device, allowing it
 * to scan for and connect to 'peripheral' devices.
 *
 * @param controller is the Bluetooth controller
 * @return status
 */
enum LB_STATUS lb_configureAsCentral(struct LB_Controller* controller);


/** Starts the discovery of peripheral devices that are presently advertising
 *
 * @param controller is the Bluetooth controller
 * @return status
 */
enum LB_STATUS lb_startDeviceDiscovery(struct LB_Controller* controller);

/** Cancels a discovery process
 *
 * @param controller is the Bluetooth controller
 * @return status
 */
enum LB_STATUS lb_stopDeviceDiscovery(struct LB_Controller* controller);

/** Called by the library when a device advertisement was observed
 *
 * @param controller is the Bluetooth controller
 * @param address is the 6-byte Bluetooth address of the device
 * @param rssi is the Receive Signal Strength Indicator
 * @param data is the advertising data
 * @param length is the size of the advertising data
 */
void lb_on_observedDeviceAdvertisment(struct LB_Controller* controller, const uint8_t* address, int8_t rssi, const uint8_t* data, uint8_t length);

/** Called by the library when a device discovery interval has completed
 *
 * @param controller is the Bluetooth controller
 */
void lb_on_deviceDiscoveryComplete(struct LB_Controller* controller);


/** Creates a connection to a device
 *
 * @param controller is the Bluetooth controller
 * @param address is the 6-byte Bluetooth address of the device
 * @param[out] device will contain the device reference
 * @return status
 */
enum LB_STATUS lb_openDeviceConnection(struct LB_Controller* controller, const uint8_t* address, struct LB_Device** device);

/** @}
 *
 * @defgroup lightBLUE_device Device Interface
 *
 * @{
 */

/** Closes a connection to a Bluetooth device
 *
 * @param device is the Bluetooth device
 * @return status
 */
enum LB_STATUS lb_closeDeviceConnection(struct LB_Device* device);

/** Called by the library when a device was disconnected from the controller
 *
 * @param device is the Bluetooth device
 * @param reason is the HCI status code
 */
void lb_on_disconnectedFromDevice(struct LB_Device* device, enum HCI_StatusCode reason);


/** Starts enumerating the primary services on a connected device
 *
 * @param device is the Bluetooth device
 * @return status
 */
enum LB_STATUS lb_startServiceDiscovery(struct LB_Device* device);

/** Called by the library when a primary service was observed
 *
 * @param device is the Bluetooth device
 * @param attributeHandle the handle to the service
 * @param groupEndHandle the handle one beyond last attribute in the service
 * @param attribute is the service UUID
 * @param attributeLength is the length of the service UUID
 */
void lb_on_discoveredPrimaryService(struct LB_Device* device, uint16_t attributeHandle, uint16_t groupEndHandle, const uint8_t* attribute, uint8_t attributeLength);

/** Sets the value of a character attribute on a connected device
 *
 * @param device is the Bluetooth device
 * @param attributeHandle is the handle of the attribute
 * @param attributeValue is the new value of the attribute
 * @param attributeLength is the size of the new value
 * @return status
 */
enum LB_STATUS lb_writeCharValue(struct LB_Device* device, uint16_t attributeHandle, const uint8_t* attributeValue, uint8_t attributeLength);

/** Retrieves the value of a character attribute on a connected device
 *
 * @param device is the Bluetooth device
 * @param attributeHandle is the handle of the attribute
 * @param[out] attributeValue will receive the value of the attribute
 * @param attributeCapacity is the size of the attribute value buffer
 * @param[out] attributeLength is the size of the value
 * @return status
 */
enum LB_STATUS lb_readCharValue(struct LB_Device* device, uint16_t attributeHandle, uint8_t* attributeValue, uint8_t attributeCapacity, uint8_t* attributeLength);


/** Called by the library when it receives an attribute notification
 *
 * @param device is the Bluetooth device
 * @param attributeHandle is the handle of the attribute
 * @param status is the HCI status
 * @param attributeValue will receive the value of the attribute
 * @param attributeLength is the size of the value
 */
void lb_on_receivedNotification(struct LB_Device* device, uint16_t attributeHandle, uint8_t status, const uint8_t* attributeValue, uint8_t attributeLength);

/** @}
 *
 * @}
 */

#endif // __COMMANDS_H__

