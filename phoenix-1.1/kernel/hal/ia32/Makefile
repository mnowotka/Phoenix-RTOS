#
# Makefile for Phoenix-RTOS kernel IA32 HAL
#
# Copyright 2001, 2005 Pawel Pisarczyk
#

ASMS = init.S intrstubs.S
SRCS = gdtidt.c pmap.c console.c interrupts.c timedev.c archcont.c db_disasm.c
OBJS = $(SRCS:.c=.o) $(ASMS:.S=.o)


all: check init.o $(ARCH)


check:
	@(if test -z "$(ARCH)"; then\
		echo "Error: Only main Makefile can be used for compilation!";\
		exit 1;\
	fi;)


.c.o:
	@(echo "CC $<"; $(CC) -c $(CFLAGS) $<)


.S.o:
	@(echo "ASM $<"; $(CC) -c $(CFLAGS) $<)


include .depend


$(ARCH): $(OBJS)
	$(AR) $(ARFLAGS) $(ARCH) $(OBJS)


depend: check
	$(MKDEP) $(MKDEPFLAGS) $(SRCS)


clean: check
	rm -f core *.o $(ARCH)
