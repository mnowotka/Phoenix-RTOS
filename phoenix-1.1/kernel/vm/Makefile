#
# Makefile for Phoenix-RTOS VM subsystem
#
# Copyright 2001, 2005 Pawel Pisarczyk
#

SRCS = vm.c kmalloc.c
OBJS = $(SRCS:.c=.o)


all: check $(ARCH)


check:
	@(if test -z "$(ARCH)"; then\
		echo "Error: Only main Makefile can be used for compilation!";\
		exit 1;\
	fi;)


.c.o:
	@(echo CC $<; $(CC) -c $(CFLAGS) $<)


include .depend


$(ARCH): $(OBJS)
	$(AR) $(ARFLAGS) $(ARCH) $(OBJS)


depend: check
	$(MKDEP) $(MKDEPFLAGS) $(SRCS)


clean: check
	rm -f core *.o $(ARCH)
