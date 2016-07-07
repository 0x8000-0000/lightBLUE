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

#ifndef __SENSOR_TAG_H__
#define __SENSOR_TAG_H__

#include <stdbool.h>
#include <stdint.h>

struct LB_Device;

bool SensorTag_enableBarometer(struct LB_Device* device, bool enable);

bool SensorTag_readBarometerData(struct LB_Device* device, float* temperature_C, unsigned* pressure_Pa);

bool SensorTag_enableIMU(struct LB_Device* device, bool enable);

bool SensorTag_enableIMUNotifications(struct LB_Device* device, bool enable);

struct threeDvector
{
   int16_t x;
   int16_t y;
   int16_t z;
};

bool SensorTag_readIMUData(struct LB_Device* device, struct threeDvector* gyro, struct threeDvector* accel, struct threeDvector* mag);

#endif // __SENSOR_TAG_H__

