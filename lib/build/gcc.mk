CC:=gcc
LD:=gcc

CFLAGS:=-std=c11 -Wall -Og -g -MMD -Wmissing-prototypes -Werror
LFLAGS:=-g

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	$(CC) $(CFLAGS) -E -o $@ $<


