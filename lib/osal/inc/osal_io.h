/**
 * @file osal_io.h
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

#ifndef __OSAL_IO_H__
#define __OSAL_IO_H__

#include <stdint.h>

/** @addtogroup OSAL OS Abstraction layer
 *
 * @{
 *
 * @defgroup OSAL_io I/O Interface
 *
 * @{
 */

/** Opaque structure representing a data channel
 */
struct io_channel;

/** Enables I/O debugging
 */
void io_setDebugLevel(uint32_t value);

/** Retrieves user data associated with channel
 *
 * @param channel is the channel
 * @return the user pointer associated with the channel
 */
void* io_getUserPtr(struct io_channel* channel);

/** Removes the channel association and closes the port
 *
 * @param channel is the i/o channel
 */
void io_closePort(struct io_channel* channel);

/** Sends data on a channel, asynchronously
 *
 * @note The buffer must not be freed or changed until io_on_transmissionComplete is
 * called, or io_waitForTransmitComplete has returned.
 *
 * @param channel is the i/o channel
 * @param buffer contains the data to be send
 * @param length is the amount of data to be sent
 *
 * @see io_on_transmissionComplete
 * @see io_waitForTransmitComplete
 */
void io_sendData(struct io_channel* channel, const uint8_t* buffer, uint32_t length);

/** Called by the library when all the scheduled data has been received by the OS
 *
 * @param channel is the i/o channel
 */
void io_on_transmissionComplete(struct io_channel* channel);

/** Blocks the calling thread until all the data scheduled to be sent on the
 * given channel has been received by the OS.
 *
 * @param channel is the i/o channel
 */
void io_waitForTransmitComplete(struct io_channel* channel);

/** Event called from the library runtime when data has been received
 *
 * @param channel is the channel on which the data has been received
 * @param buffer contains the received data
 * @param length contains the size of received data
 */
void io_on_dataReceived(struct io_channel* channel, const uint8_t* buffer, uint32_t length);

/** @}
 *
 * @}
 */

#endif // __OSAL_IO_H__

