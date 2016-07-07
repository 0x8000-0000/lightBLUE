/**
 * @file discover_services.c
 * @brief Connect to a given Bluetooth device and list services
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

#include <osal_core.h>
#include <osal_io.h>

#include <hci.h>
#include <controller.h>
#include <commands.h>
#include <gap.h>
#include <utils.h>

void lb_on_observedDeviceAdvertisment(struct LB_Controller* controller, const uint8_t* address, int8_t rssi, const uint8_t* data, uint8_t length)
{
}

void lb_on_deviceDiscoveryComplete(struct LB_Controller* controller)
{
}

static struct LB_Device* peerConnectionHandle = NULL;

void lb_on_disconnectedFromDevice(struct LB_Device* device, enum HCI_StatusCode reason)
{
   printf("Device disconnected: %p\n", device);
   assert(peerConnectionHandle == device);
   peerConnectionHandle = 0;
}

void lb_on_discoveredPrimaryService(struct LB_Device* device, uint16_t attributeHandle, uint16_t groupEndHandle, const uint8_t* attribute, uint8_t attributeLength)
{
   printf("H: %p   [Start: %04x - End: %04x] -> ", device, attributeHandle, groupEndHandle);
   utl_printUUID(attribute, attributeLength);
   putchar('\n');
}

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      puts("Serial port missing");
      return 1;
   }

   if (argc < 3)
   {
      puts("Bluetooth address missing");
      return 1;
   }

   uint8_t peerAddress[6];
   if (! utl_parseAddress(argv[2], peerAddress))
   {
      printf("Failed to parse input address: %s\n", argv[2]);
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

   if (LB_OK != lb_openDeviceConnection(controller, peerAddress, &peerConnectionHandle))
   {
      goto done;
   }

   printf("Connected to ");
   utl_printAddress(peerAddress);
   printf(" using: %p\n", peerConnectionHandle);

   enum LB_STATUS status = lb_startServiceDiscovery(peerConnectionHandle);
   if (LB_OK == status)
   {
      printf("H: %p   All services discovered\n", peerConnectionHandle);
   }
   else
   {
      printf("Service discovery failed: %u\n", status);
   }

   lb_closeDeviceConnection(peerConnectionHandle);

   os_sleep_ms(2000);

done:

   lb_disconnect(controller);

   lb_cleanup();

   return 0;
}

void lb_on_receivedNotification(struct LB_Device* device, uint16_t attributeHandle, uint8_t status, const uint8_t* attributeValue, uint8_t attributeLength)
{
}

