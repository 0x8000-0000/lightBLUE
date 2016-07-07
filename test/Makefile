# @file Makefile
# @brief Makefile for test applications
# @author Florin Iucha <florin@signbit.net>
# @copyright Apache License, Version 2.0

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This file is part of LightBLUE Bluetooth Smart Library

include ../lib/build/platform.mk
include ../lib/build/gcc.mk

include ../lib/osal/lib.mk
include ../lib/sensor_tag/lib.mk

APPS:=get_version$(EXE) discover_devices$(EXE) \
	test_connect$(EXE) parse_address$(EXE) \
	discover_services$(EXE) \
	sensor_tag_barometer$(EXE) sensor_tag_imu$(EXE)

all: $(APPS)

CFLAGS+=-I../inc $(LIBS_CFLAGS)

SOURCES:=$(notdir $(wildcard ../src/*.c) $(wildcard *.c) $(LIBS_SOURCES))
OBJECTS:=$(SOURCES:.c=.o)
DEPS:=$(SOURCES:.c=.d)

LIGHT_BLUE_OBJECTS:=commands.o controller.o utils.o \
	hci.o gap.o hci_text.o \
	st_aci.o ti_hci.o

get_version$(EXE): get_version.o $(LIGHT_BLUE_OBJECTS) $(LIBS_OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^

discover_devices$(EXE): discover_devices.o $(LIGHT_BLUE_OBJECTS) $(LIBS_OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^

discover_services$(EXE): discover_services.o $(LIGHT_BLUE_OBJECTS) $(LIBS_OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^

test_connect$(EXE): test_connect.o $(LIGHT_BLUE_OBJECTS) $(LIBS_OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^

sensor_tag_barometer$(EXE): sensor_tag_barometer.o sensor_tag.o $(LIGHT_BLUE_OBJECTS) $(LIBS_OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^

sensor_tag_imu$(EXE): sensor_tag_imu.o sensor_tag.o $(LIGHT_BLUE_OBJECTS) $(LIBS_OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^

parse_address$(EXE): parse_address.o utils.o
	$(LD) $(LFLAGS) -o $@ $^

.PHONY: all clean

clean:
	$(RM) $(APPS) $(OBJECTS) $(DEPS)

vpath %.c \
	../src \
	$(LIBS_VPATH)

-include $(DEPS)

