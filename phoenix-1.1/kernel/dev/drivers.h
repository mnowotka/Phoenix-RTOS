/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Device driver interface
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

#ifndef _DRIVERS_H_
#define _DRIVERS_H_


#include <hal/current/types.h>


#define DEV_MAX_MAJOR   64
#define DEV_MAX_MINOR   64


/* Global ioctl operations */
#define SET_DISCIP      0


/* Device descriptor */
typedef struct dev {
	uint_t major;
	uint_t minor;
} dev_t;


/* Driver file operations - main structure of every driver in system */
typedef struct fops {	
	int (*open)(dev_t *dev);
	int (*write)(dev_t *dev, char *buff, uint_t *pos, uint_t length);
	int (*read)(dev_t *dev, char *buff, uint_t *pos, uint_t length);
	int (*ioctl)(dev_t *dev, uint_t func, int *val);
	int (*seek)(dev_t *dev, int rpos, uint_t mode);
	void (*close)(dev_t *dev);
} fops_t;


/* Function registers new device */
extern int drivers_register(uint_t major, uint_t minor, fops_t *fops);


/* Function initializes device driver interface */
extern void drivers_init(void);


/* Interface functions */
extern int dev_open(dev_t *dev);
extern int dev_read(dev_t *dev, char *buff, int *pos, int length);
extern int dev_write(dev_t *dev, char *buff, int *pos, int length);
extern int dev_ioctl(dev_t *dev, uint_t func, int *val);


#endif
