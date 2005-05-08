#
# Phoenix-RTOS loader, version: 2.0
# (c) Pawel Pisarczyk, 2004
#

CC = bcc
CFLAGS = -ansi -I.
AS = as86
LD = ld86

SRCS = plo.c low.c plostd.c cmd.c timer.c serial.c msg.c phfs.c
OBJS = _plo.o _lib.o _low.o $(SRCS:.c=.o)

all: plo

.c.o:
	$(CC) -c $(CFLAGS) $<

.S.o:
	$(AS) -o $*.o $<
	
plo: $(OBJS)
	$(LD) -T0 -d -o plo.bin $(OBJS)

clean:
	rm -f *.o *~ core *.s
