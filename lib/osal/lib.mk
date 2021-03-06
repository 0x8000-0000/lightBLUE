# @file lib.mk
# @brief OSAL library makefile fragment
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

OSAL_PATH:=$(dir $(lastword $(MAKEFILE_LIST)))

ifeq ($(OS),Windows_NT)
	LIBS_CFLAGS+=-D_WIN32_WINNT=_WIN32_WINNT_WIN8
endif

LIBS_CFLAGS+=-I$(OSAL_PATH)inc
LIBS_SOURCES+=$(wildcard $(OSAL_PATH)src/*_$(PLATFORM).c)
LIBS_OBJECTS+=$(notdir $(LIBS_SOURCES:.c=.o))
LIBS_VPATH+=$(OSAL_PATH)src

