#
# Phoenix system binaries and test programs
# (c) Pawel Pisarczyk, 2001
#

CC = gcc
LD = ld

INCLUDE = -I../libph

CFLAGS = $(INCLUDE) -fomit-frame-pointer\
         -fno-strength-reduce -Wstrict-prototypes -O2 -Wall -nostartfiles -nostdlib

LIBDIR = ../libph

.c.o:
	$(CC) -c $(CFLAGS) $<

all: psh sig exc0 exc13 exc14

libph:
	@(cd ../libph; make all; cd ../sys)

psh: libph lex.o psh.o
	$(LD) $(LDFLAGS) -Ttext 0x2000 -Tdata 0x60000 -o psh lex.o psh.o $(LIBDIR)/libph.a

sig: libph sig.o
	$(LD) $(LDFLAGS) -Ttext 0x10000 -Tdata 0x60000 -o sig sig.o $(LIBDIR)/libph.a

exc0: libph exc0.o
	$(LD) $(LDFLAGS) -Ttext 0x10000 -Tdata 0x60000 -o exc0 exc0.o $(LIBDIR)/libph.a

exc13: libph exc13.o
	$(LD) $(LDFLAGS) -Ttext 0x10000 -Tdata 0x60000 -o exc13 exc13.o $(LIBDIR)/libph.a

exc14: libph exc14.o
	$(LD) $(LDFLAGS) -Ttext 0x10000 -Tdata 0x60000 -o exc14 exc14.o $(LIBDIR)/libph.a

clean:
	rm -f *.o *~ core *.s
