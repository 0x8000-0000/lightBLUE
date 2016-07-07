/**
 * @file controller.h
 * @brief Serial controller interface
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

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

/** @addtogroup lightBLUE lightBLUE
 *
 * @{
 *
 * @defgroup lightBLUE_lib Initialization
 *
 * @{
 */

/** Initialize the lightBLUE library
 */
int lb_initialize(void);

/** Un-initialize the lightBLUE library
 */
void lb_cleanup(void);

/** @}
 *
 * @addtogroup lightBLUE_controller Controller Interface
 *
 * @{
 */

/** Opaque object representing a connection to a Bluetooth controller
 */
struct LB_Controller;

/** Connect to a Bluetooth controller
 *
 * @param portName is the name of the serial port
 * @return a pointer to a controller object
 */
struct LB_Controller* lb_connect(const char* portName);

/** Disconnect from a Bluetooth controller
 *
 * @param controller is a pointer to a controller object
 */
void lb_disconnect(struct LB_Controller* controller);

/** @}
 *
 * @}
 */

#endif // __CONNECTION_H__

