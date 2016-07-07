/**
 * @file gap.c
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

#include <assert.h>
#include <stdio.h>

#include <gap.h>

/*
 * Length / Type / Value... / Length / Type / Value
 */
void gap_decodeAdvertisingData(const uint8_t* advertisingData, uint8_t advertisingDataLength)
{
   const uint8_t* const advertisingDataEnd = advertisingData + advertisingDataLength;
   while (advertisingDataEnd > advertisingData)
   {
      uint8_t length = *advertisingData;
      advertisingData ++;
      uint8_t type   = *advertisingData;
      const uint8_t* const value = advertisingData + 1;
      advertisingData += length;

      switch (type)
      {
         case AD_TYPE_FLAGS:
            assert(2 == length);
            printf(" [Flags = %x]", *value);
            break;

         case AD_TYPE_16_BIT_SERV_UUID:
            assert(3 == length);
            printf(" [UUID16 = %x]", value[0] | (((uint16_t) value[1]) << 8));
            break;

         case AD_TYPE_SHORTENED_LOCAL_NAME:
            printf(" [S.Name = ");
            for (uint8_t ii = 0; ii < (length - 1); ii ++)
            {
               putchar(value[ii]);
            }
            putchar(']');
            break;

         case AD_TYPE_COMPLETE_LOCAL_NAME:
            printf(" [C.Name = ");
            for (uint8_t ii = 0; ii < (length - 1); ii ++)
            {
               putchar(value[ii]);
            }
            putchar(']');
            break;

         case AD_TYPE_TX_POWER_LEVEL:
            {
               int8_t txPower = *((int8_t*) value);
               printf(" [TX Power = %d dbm]", txPower);
            }
            break;

         case AD_TYPE_SLAVE_CONN_INTERVAL:
            assert(5 == length);
            {
               uint16_t connMin = value[0] | (((uint16_t) value[1]) << 8);
               uint16_t connMax = value[2] | (((uint16_t) value[3]) << 8);
               printf(" [Slave interval %.2lf - %.2lf ms]", 1.25f * connMin, 1.25f * connMax);
            }
            break;

         case AD_TYPE_MANUFACTURER_SPECIFIC_DATA:
            {
               uint16_t manufacturerId = value[0] | (((uint16_t) value[1]) << 8);
               printf(" [Manufacturer %u]", manufacturerId);
            }
            break;

         default:
            printf(" [AD %x]", type);
            break;
      }
   }
}

