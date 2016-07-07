/**
 * @file utils.c
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

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <utils.h>

void utl_printBuffer(const uint8_t* buffer, uint32_t length)
{
   printf("[%u:", length);
   for (uint32_t ii = 0; ii < length; ii ++)
   {
      if (0 == ii % 8)
      {
         putchar(' ');
      }

      printf(" %02x", buffer[ii]);
   }
   putchar(']');
}

void utl_printAddress(const uint8_t* buffer)
{
   for (uint32_t ii = 0; ii < 6; ii ++)
   {
      if (ii)
      {
         putchar(':');
      }
      printf("%02x", buffer[5 - ii]);
   }
}

bool utl_parseAddress(const char* buffer, uint8_t* address)
{
   memset(address, 0, 6);

   bool inputIsValid = true;

   uint32_t bufferLen = 0;

   uint8_t nibble = 0;

   while (inputIsValid && (bufferLen < 18))
   {
      const uint8_t byte = buffer[bufferLen];

      if (0 == byte)
      {
         break;
      }

      switch ((bufferLen + 1) % 3)
      {
         case 0:
            if (':' != byte)
            {
               inputIsValid = false;
            }
            break;

         case 1:
            if (('0' <= byte) && (byte <= '9'))
            {
               nibble = (byte - '0') << 4;
            }
            else if (('a' <= byte) && (byte <= 'f'))
            {
               nibble = (byte - 'a' + 10) << 4;
            }
            else if (('A' <= byte) && (byte <= 'F'))
            {
               nibble = (byte - 'A' + 10) << 4;
            }
            else
            {
               inputIsValid = false;
            }
            break;

         case 2:
            if (('0' <= byte) && (byte <= '9'))
            {
               nibble |= (byte - '0');
            }
            else if (('a' <= byte) && (byte <= 'f'))
            {
               nibble |= (byte - 'a' + 10);
            }
            else if (('A' <= byte) && (byte <= 'F'))
            {
               nibble |= (byte - 'A' + 10);
            }
            else
            {
               inputIsValid = false;
            }
            if (inputIsValid)
            {
               //printf("BufferLen: %u   Byte: %02x\n", bufferLen, nibble);
               address[5 - (bufferLen / 3)] = nibble;
               nibble = 0;
            }
            break;

         default:
            assert(false);
            break;
      }

      bufferLen ++;
   }

   if (! inputIsValid)
   {
      return false;
   }

   if (17 != bufferLen)
   {
      return false;
   }

   return true;
}

void utl_printUUID(const uint8_t* buffer, uint8_t length)
{
   if (2 == length)
   {
      printf("%02x%02x", buffer[1], buffer[0]);
   }
   else if (16 == length)
   {
      for (uint32_t ii = 0; ii < 16; ii ++)
      {
         printf("%02x", buffer[15 - ii]);
         if ((3 == ii) || (5 == ii) || (7 == ii) || (9 == ii))
         {
            putchar('-');
         }
      }
   }
   else
   {
      assert(false);
   }
}
