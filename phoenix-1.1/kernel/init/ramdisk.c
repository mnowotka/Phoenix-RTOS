/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Simple ramdisk
 *
 * Copyright 2002 Pawel Pisarczyk
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

#include <hal/current/locore.h>
#include <hal/current/types.h>
#include <hal/current/defs.h>
#include <hal/current/interrupts.h>
#include <hal/current/timedev.h>
#include <hal/current/archcont.h>
#include <vm/vm.h>
#include <vm/kmalloc.h>
#include <init/std.h>
#include <task/task.h>
#include <init/ramdisk.h>


ramdisk_t *kernel_ramdisk = (ramdisk_t *)RAMDISK_VADDR;


/* Function retrieves information about ramdisk */
int get_ramdisk_info(ramdisk_t *rd, int *err)
{		
	/* Test if ramdisk exist */
	if (kernel_ramdisk->id[0] != 0x7f) {
		*err = -1;
		return -1;
	}
	
	*err = 0;
	memcpy(rd, kernel_ramdisk, sizeof(ramdisk_t));
	return 0;
}
