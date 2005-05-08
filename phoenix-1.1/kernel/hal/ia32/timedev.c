/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Low-level system timer driver
 *
 * Copyright 2001 Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * Phoenix-RTOS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Phoenix-RTOS kernel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phoenix-RTOS kernel; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <hal/current/types.h>
#include <hal/current/defs.h>
#include <hal/current/console.h>
#include <hal/current/interrupts.h>
#include <init/std.h>


/* Function initializes system timer. Slice parameter defines clock cycle in in microseconds */
void timedev_init(uint_t slice)
{
	uint_t t;
	
	std_printf("timedev: setting time slice to %d us\n", slice);
	
	t = slice * 1200 / 1000;
	
	std_printf("timedev: t=%d\n", t);
	
	/* First generator, operation - CE write, work mode 2, binary counting */
	bus_outb(0x43, 0x34);
	
	/* Set counter */
	bus_outb(0x40, (uchar_t)(t & 0xff));
	bus_outb(0x40, (uchar_t)(t >> 8));
	
	return;
}
