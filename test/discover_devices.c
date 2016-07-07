/**
 * @file discover_devices.c
 * @brief Discover nearby Bluetooth Devices
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

#include <stdio.h>

#include <osal_core.h>
#include <osal_io.h>

#include <hci.h>
#include <controller.h>
#include <commands.h>
#include <gap.h>
#include <utils.h>

void lb_on_observedDeviceAdvertisment(struct LB_Controller* controller, const uint8_t* address, int8_t rssi, const uint8_t* data, uint8_t length)
{
   printf("Device found: ");
   utl_printAddress(address);
   putchar('\n');
   printf("   RSSI: %d\n", rssi);
   printf("   Data:");
   gap_decodeAdvertisingData(data, length);
   putchar('\n');
}

void lb_on_deviceDiscoveryComplete(struct LB_Controller* controller)
{
   puts("Discovery complete.");
}

void lb_on_disconnectedFromDevice(struct LB_Device* device, enum HCI_StatusCode reason)
{
}

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      puts("Serial port missing");
      return 1;
   }

   if (lb_initialize() < 0)
   {
      puts("Failed to initialize lightBLUE library");
      return 2;
   }

   io_setDebugLevel(0);
   lb_setDebugLevel(0);

   struct LB_Controller* controller = lb_connect(argv[1]);

   if (! controller)
   {
      printf("Failed to connect to %s.\n", argv[1]);
      return 3;
   }

   if (LB_OK != lb_initializeHCI(controller))
   {
      goto done;
   }

   if (LB_OK != lb_configureAsCentral(controller))
   {
      goto done;
   }

   if (LB_OK != lb_startDeviceDiscovery(controller))
   {
      goto done;
   }

   puts("Waiting for events. Press Ctrl-C to quit.");
   os_waitForKeyboardInterrupt();

done:

   lb_disconnect(controller);

   lb_cleanup();

   return 0;
}

void lb_on_discoveredPrimaryService(struct LB_Device* device, uint16_t attributeHandle, uint16_t groupEndHandle, const uint8_t* attribute, uint8_t attributeLength)
{
}

void lb_on_receivedNotification(struct LB_Device* device, uint16_t attributeHandle, uint8_t status, const uint8_t* attributeValue, uint8_t attributeLength)
{
}

