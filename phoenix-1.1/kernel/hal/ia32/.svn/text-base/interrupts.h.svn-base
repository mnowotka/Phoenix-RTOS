/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Exception and interrupt handling
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

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include <hal/current/types.h>
#include <hal/current/defs.h>


#pragma pack(1)

typedef struct {
	u32 edi;
  u32 esi;
  u32 ebp;
  u32	edx;
  u32	ecx;
  u32 ebx;
  u32 eax;
  u16 gs;
	u16 fs;
  u16 es;
  u16 ds;
  u32 err;
  u32 eip;
  u32 cs;
  u32 eflags;
  u32 esp;
  u32 ss;
} exc_context_t;

#pragma pack(4)


/* Function setups handler for specified interrupt */
extern void set_intr_handler(uint_t intr, void *handler); 


/* Function setups handler for specified exception */
extern void set_exc_handler(uint_t exc, void *handler);


/* Function initializes interrupt and exception handling */
extern void interrupts_init(void);


#endif
