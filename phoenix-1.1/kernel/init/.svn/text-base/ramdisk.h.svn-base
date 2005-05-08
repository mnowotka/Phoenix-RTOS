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

#ifndef _RAMDISK_H_
#define _RAMDISK_H_


/* Ramdisk parameters */
#define RAMDISK_MAX_FILES      32
#define RAMDISK_FILENAME_SIZE  12


/* Ramdisk memory position */
#define RAMDISK_VADDR    (KERNEL_BASE + 0x80000)


typedef struct _ramdisk_file_t {
	char         name[RAMDISK_FILENAME_SIZE];
	unsigned int offset;
} ramdisk_file_t;


typedef struct _ramdisk_t {
	char           id[8];
	ramdisk_file_t files[RAMDISK_MAX_FILES];
} ramdisk_t;


/* Function retrieves information about ramdisk */
extern int get_ramdisk_info(ramdisk_t *rd, int *err);


#endif
