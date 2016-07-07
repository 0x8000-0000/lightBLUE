/**
 * @file osal_serial.h
 * @brief Serial port communication interface
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

#ifndef __OSAL_SERIAL_H__
#define __OSAL_SERIAL_H__

#include <osal_io.h>

/** @addtogroup OSAL OS Abstraction layer
 *
 * @{
 *
 * @defgroup OSAL_serial Serial I/O
 *
 * @{
 */

/** Opaque structure representing a data channel
 */
struct io_channel;

/** Opens a serial port and associates with a channel
 *
 * @param portName is the serial device; passed straight to OS
 * @param baudRate is the baud rate; passed straight to OS
 * @param userPtr is a pointer to user data structure
 * @return an initialized channel
 *
 * @see io_getUserPtr
 */
struct io_channel* io_openSerialPort(const char* portName, uint32_t baudRate, void* userPtr);

/** @}
 *
 * @}
 */

#endif // __OSAL_SERIAL_H__

