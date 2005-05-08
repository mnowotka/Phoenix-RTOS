#
# Makefile for Phoenix-RTOS kernel
#
# Copyright 2001, 2005 Pawel Pisarczyk
#

SRCDIR := $(shell pwd)
SUBDIRS = hal/current init vm task dev comm phfs

CC = gcc
CFLAGS = -O2 -Wall -I$(SRCDIR) -nostartfiles -nostdlib -fomit-frame-pointer\
         -fno-strength-reduce -Wstrict-prototypes -DVERSION=\"$(VERSION)\"

MKDEP = mkdep
MKDEPFLAGS = $(CFLAGS)

AR = ar
ARFLAGS = -r

LD = ld
LDFLAGS = 

ARCH = code.a
ARCHS := $(shell for i in $(SUBDIRS); do echo "$$i/$(ARCH)"; done)

BIN = phoenix
VADDR = c0060000

export CC CFLAGS MKDEP MKDEPFLAGS AR ARFLAGS ARCH


all: subsystems $(BIN)


subsystems:
	@for i in $(SUBDIRS); do\
		d=`pwd`;\
		if ! cd $$i; then\
			exit 1;\
		fi;\
		if ! make; then\
			exit 1;\
		fi;\
		cd $$d;\
	done;


$(BIN): hal/current/init.o $(ARCHS)
	$(LD) $(LDFLAGS) -Ttext $(VADDR) -o $(BIN) hal/current/init.o -\( $(ARCHS) -\)
	@(echo "";\
	echo "=>Phoenix-RTOS kernel binary has been successfully created";\
	echo "")


depend:
	@for i in $(SUBDIRS); do\
		d=`pwd`;\
		if ! cd $$i; then\
			exit 1;\
		fi;\
		if ! make depend; then\
			exit 1;\
		fi;\
		cd $$d;\
	done;


clean:
	rm -f core *.o $(BIN)
	@for i in $(SUBDIRS); do\
		d=`pwd`;\
		if ! cd $$i; then\
			exit 1;\
		fi;\
		if ! make clean; then\
			exit 1;\
		fi;\
		cd $$d;\
	done;
