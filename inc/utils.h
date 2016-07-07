/**
 * @file utils.h
 * @brief Simple buffer utilities
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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>
#include <stdint.h>

/** @addtogroup lightBLUE lightBLUE
 *
 * @{
 *
 * @addtogroup lightBLUE_utils Debugging Utilities
 *
 * @{
 */

/** Prints the contents of the buffer in hexadecimal on stdout
 *
 * @param buffer contains the data
 * @param length is the size of data
 */
void utl_printBuffer(const uint8_t* buffer, uint32_t length);

/** Prints a Bluetooth address in hexadecimal on stdout
 *
 * @param address contains a 6-byte Bluetooth address
 */
void utl_printAddress(const uint8_t* address);

/** Prints a Bluetooth UUID in hexadecimal on stdout
 *
 * @param buffer contains the UUID
 * @param length indicates the size of UUID (2 or 16 bytes)
 */
void utl_printUUID(const uint8_t* buffer, uint8_t length);

/** Parse "AA:BB:CC:DD:EE:FF" into 6 byte address
 *
 * @param[in] buffer points to the textual representation of the address
 * @param[out] address contains the 6-byte Bluetooth address
 * @return true if address is valid
 */
bool utl_parseAddress(const char* buffer, uint8_t* address);

/** @}
 *
 * @}
 */

#endif // __UTILS_H__

