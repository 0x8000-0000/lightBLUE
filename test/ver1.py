#!/usr/bin/python

import sys
import serial
import time

with serial.Serial(sys.argv[1], 115200, timeout = 1) as ser:
   request = bytearray([0x01, 0x01, 0x10, 0x00])
   ser.write(request)
   #time.sleep(1)
   response = ser.read(32)
   print(''.join(" %02x" % b for b in response))
