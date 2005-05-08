/*
 * Phoenix-RTOS
 *
 * Standard library
 *
 * Kernel device drivers access routines
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

#include <libph.h>
#include <arch/locore.h>


/* Kernel devices */
dev_t dev_console = { 8, 0 };
dev_t dev_serial0 = { 16, 0 };
dev_t dev_serial1 = { 16, 1 };
dev_t dev_serial2 = { 16, 2 };
dev_t dev_serial3 = { 16, 3 };


int ph_devwrite(dev_t *dev, char *buff, uint_t *pos, uint_t length)
{
	__dev_write(dev, buff, pos, length);
	return length;
}


int ph_devread(dev_t *dev, char *buff, uint_t *pos, uint_t length)
{
	__dev_read(dev, buff, pos, length);	
	return length;
}
