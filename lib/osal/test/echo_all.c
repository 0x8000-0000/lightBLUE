/**
 * @file echo.c
 * @brief Serial port communication test
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

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <osal_core.h>
#include <osal_serial.h>

void io_on_dataReceived(struct io_channel* channel, const uint8_t* buffer, uint32_t length)
{
   io_sendData(channel, buffer, length);
}

static struct io_channel* myChannel = NULL;

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      puts("COM port argument missing.");
      return 1;
   }

   if (os_initialize() < 0)
   {
      puts("Failed to initialize serial base");
      return 2;
   }

   myChannel = io_openSerialPort(argv[1], 115200, NULL);
   if (! myChannel)
   {
      printf("Failed to open port %s\n", argv[1]);
      goto done;
   }

   os_waitForKeyboardInterrupt();

done:

   if (myChannel)
   {
      io_closePort(myChannel);
   }

   os_cleanup();

   return 0;
}
