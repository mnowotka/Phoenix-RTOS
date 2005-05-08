/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * HAL interface
 *
 * Copyright 2001, 2005 Pawel Pisarczyk
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

#ifndef _HAL_H_
#define _HAL_H_


#include <hal/current/defs.h>
#include <hal/current/types.h>
#include <hal/current/locore.h>
#include <hal/current/archcont.h>
#include <hal/current/interrupts.h>
#include <hal/current/console.h>


extern int hal_disasm(void *saddr);


extern void hal_inject(void *addr, u8 mask, u8 op);


/* Function injecting fault at given location */
extern void hal_preinject(void *addr);


/* Function remove fault at given location */
extern void hal_postinject(void *addr);


#endif
