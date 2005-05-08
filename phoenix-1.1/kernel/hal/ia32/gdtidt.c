/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * GDT and IDT pseudodescriptors
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

#ifndef _GDTIDT_H_
#define _GDTIDT_H_

#include <hal/current/defs.h>
#include <hal/current/types.h>


/* IDT */
struct {
	unsigned short limit;
	unsigned int addr __attribute__((packed));	
} idt_reg = { 0x8000, KERNEL_BASE + IDT_ADDR };


/* GDT */
struct {
	unsigned short limit;
	unsigned int addr __attribute__((packed));	
} gdt_reg = { 0x8000, KERNEL_BASE + GDT_ADDR };


#endif
