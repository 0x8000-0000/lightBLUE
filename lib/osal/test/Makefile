include ../../build/platform.mk
include ../../build/gcc.mk

include ../lib.mk

APPS:=echo_all$(EXE) test_cond$(EXE) echo_plus$(EXE)

all: $(APPS)

CFLAGS+=$(LIBS_CFLAGS)

SOURCES:=$(notdir $(wildcard *.c) $(LIBS_SOURCES))
OBJECTS:=$(SOURCES:.c=.o)
DEPS:=$(SOURCES:.c=.d)

echo_all$(EXE): echo_all.o $(LIBS_OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^

echo_plus$(EXE): echo_plus.o $(LIBS_OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^

test_cond$(EXE): test_condition.o $(LIBS_OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^

.PHONY: all clean

clean:
	$(RM) $(APPS) $(OBJECTS) $(DEPS)

vpath %.c $(LIBS_VPATH)

-include $(DEPS)

