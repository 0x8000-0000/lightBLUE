/**
 * @file parse_address.c
 * @brief Test address parsing
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

#include <stdint.h>
#include <stdio.h>

#include <utils.h>

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      puts("Input address missing");
      return 1;
   }

   uint8_t address[6];

   bool success = utl_parseAddress(argv[1], address);

   if (success)
   {
      printf("Input: %s  Parsed: ", argv[1]);
      utl_printAddress(address);
      putchar('\n');
   }
   else
   {
      printf("Failed to parse input: '%s'\n", argv[1]);
   }

   return 0;
}
