/**
 * @file sensor_tag.c
 * @brief TI CC2650 SensorTag library
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
 * Based on http://www.ti.com/ww/en/wireless_connectivity/sensortag2015/tearDown.html
 */

#include <assert.h>
#include <string.h>

#include <commands.h>

#include "sensor_tag.h"

bool SensorTag_enableBarometer(struct LB_Device* device, bool enable)
{
   uint8_t enableArgument = enable;
   return LB_OK == lb_writeCharValue(device, 0x34, &enableArgument, sizeof(enableArgument));
}

bool SensorTag_readBarometerData(struct LB_Device* device, float* temperature_C, unsigned* pressure_Pa)
{
   /*
    * data[2] = (temp >> 16) & 0xFF;
    * data[1] = (temp >> 8) & 0xFF;
    * data[0] = temp & 0xFF;
    *
    * data[5] = (press >> 16) & 0xFF;
    * data[4] = (press >> 8) & 0xFF;
    * data[3] = press & 0xFF;
    */

   uint8_t barometerData[6];
   uint8_t dataLen = 0;

   if (LB_OK != lb_readCharValue(device, 0x31, barometerData, sizeof(barometerData), &dataLen))
   {
      *temperature_C = 0;
      *pressure_Pa   = 0;

      return false;
   }

   assert(sizeof(barometerData) == dataLen);

   uint32_t scaledTemperature = barometerData[0] | (((uint32_t) barometerData[1]) << 8) | (((uint32_t) barometerData[2]) << 16);

   *temperature_C = scaledTemperature / 100.0f;
   *pressure_Pa = barometerData[3] | (((uint32_t) barometerData[4]) << 8) | (((uint32_t) barometerData[5]) << 16);

   return true;
}

bool SensorTag_enableIMU(struct LB_Device* device, bool enable)
{
   uint8_t enableArgument[2] = { 0, 0 };
   if (enable)
   {
      enableArgument[0] = 0xFF;
   }
   return LB_OK == lb_writeCharValue(device, 0x3C, enableArgument, sizeof(enableArgument));
}

bool SensorTag_enableIMUNotifications(struct LB_Device* device, bool enable)
{
   uint8_t enableArgument[2] = { enable, 0 };
   return LB_OK == lb_writeCharValue(device, 0x3A, enableArgument, sizeof(enableArgument));
}

bool SensorTag_readIMUData(struct LB_Device* device, struct threeDvector* gyro, struct threeDvector* accel, struct threeDvector* mag)
{
   uint8_t rawData[18];
   uint8_t dataLen = 0;

   if (LB_OK != lb_readCharValue(device, 0x39, rawData, sizeof(rawData), &dataLen))
   {
      memset(gyro, 0, sizeof(*gyro));
      memset(accel, 0, sizeof(*accel));
      memset(mag, 0, sizeof(*mag));
      return false;
   }
   assert(sizeof(rawData) == dataLen);

   gyro->x = (int16_t) (((uint16_t) rawData[0]) | (((uint16_t) rawData[1]) << 8));
   gyro->y = (int16_t) (((uint16_t) rawData[2]) | (((uint16_t) rawData[3]) << 8));
   gyro->z = (int16_t) (((uint16_t) rawData[4]) | (((uint16_t) rawData[5]) << 8));

   accel->x = (int16_t) (((uint16_t) rawData[6]) | (((uint16_t) rawData[7]) << 8));
   accel->y = (int16_t) (((uint16_t) rawData[8]) | (((uint16_t) rawData[9]) << 8));
   accel->z = (int16_t) (((uint16_t) rawData[10]) | (((uint16_t) rawData[11]) << 8));

   mag->x = (int16_t) (((uint16_t) rawData[12]) | (((uint16_t) rawData[13]) << 8));
   mag->y = (int16_t) (((uint16_t) rawData[14]) | (((uint16_t) rawData[15]) << 8));
   mag->z = (int16_t) (((uint16_t) rawData[16]) | (((uint16_t) rawData[17]) << 8));

   return true;
}
