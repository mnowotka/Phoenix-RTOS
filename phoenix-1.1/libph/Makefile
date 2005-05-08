#
# Phoenix-RTOS
#
# Phoenix-RTOS standard library
# (c) Pawel Pisarczyk, 2001
#

CC = gcc
LD = ld
INCLUDE = -I.
CFLAGS = $(INCLUDE) -fomit-frame-pointer\
         -fno-strength-reduce -Wstrict-prototypes -O2 -Wall -nostartfiles -nostdlib

SRCS = printf.c dev.c sys.c
OBJS = $(SRCS:.c=.o)

.c.o:
	$(CC) -c $(CFLAGS) $<

all: libc

libc: $(OBJS)
	ar r libph.a $(OBJS)

clean:
	rm -f *.o *~ core *.s
