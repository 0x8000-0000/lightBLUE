/**
 * @file initialize.c
 * @brief Initialize HCI for attached network processor
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

#include <osal_io.h>

#include <hci.h>
#include <controller.h>
#include <commands.h>

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

   io_setDebugLevel(5);

   struct LB_Controller* controller = lb_connect(argv[1]);

   if (! controller)
   {
      printf("Failed to connect to %s.\n", argv[1]);
      return 3;
   }

   if (LB_OK == lb_resetHCI(controller))
   {
      puts("HCI successfully reset on device");
   }
   else
   {
      puts("Failed initialize HCI");
      goto done;
   }

   struct HCI_RESPONSE_Read_Local_Version_Information version;

   if (LB_OK == lb_readLocalVersionInformation(controller, &version))
   {
      printf("Manufacturer id: %02x\n", (unsigned int) uint16Value(&version.manufacturerId));
   }
   else
   {
      puts("Failed to read network processor version.");
   }

   puts("All done; shutting down.");

done:

   lb_disconnect(controller);

   lb_cleanup();

   return 0;
}

void lb_on_observedDeviceAdvertisment(struct LB_Controller* controller, const uint8_t* address, int8_t rssi, const uint8_t* data, uint8_t length)
{
}

void lb_on_deviceDiscoveryComplete(struct LB_Controller* controller)
{
}

void lb_on_disconnectedFromDevice(struct LB_Device* device, enum HCI_StatusCode reason)
{
}

void lb_on_discoveredPrimaryService(struct LB_Device* device, uint16_t attributeHandle, uint16_t groupEndHandle, const uint8_t* attribute, uint8_t attributeLength)
{
}

void lb_on_receivedNotification(struct LB_Device* device, uint16_t attributeHandle, uint8_t status, const uint8_t* attributeValue, uint8_t attributeLength)
{
}

