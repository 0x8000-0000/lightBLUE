/**
 * @file gap.h
 * @brief Generic Access Profile definitions
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
 *    Core Specification Supplement Rev 6
 */

#ifndef __GAP_H__
#define __GAP_H__

#include <stdint.h>

/** @addtogroup lightBLUE lightBLUE
 *
 * @{
 *
 * @addtogroup lightBLUE_utils Debugging Utilities
 *
 * @{
 */

/** GAP Advertising data, defined in Bluetooth Core Specification v6
 */
enum GAP_AdvertisingData
{
   AD_TYPE_FLAGS                       = 0x01,
   AD_TYPE_16_BIT_SERV_UUID            = 0x02,
   AD_TYPE_SHORTENED_LOCAL_NAME        = 0x08,
   AD_TYPE_COMPLETE_LOCAL_NAME         = 0x09,
   AD_TYPE_TX_POWER_LEVEL              = 0x0A,
   AD_TYPE_SLAVE_CONN_INTERVAL         = 0x12,
   AD_TYPE_MANUFACTURER_SPECIFIC_DATA  = 0xFF,
};

/** Decodes advertising packet to stdout
 *
 * @param advertisingData is the payload of the advertisement packet
 * @param length is the length of the advertisement data
 */
void gap_decodeAdvertisingData(const uint8_t* advertisingData, uint8_t length);

/** @}
 *
 * @}
 */

#endif // __GAP_H__

