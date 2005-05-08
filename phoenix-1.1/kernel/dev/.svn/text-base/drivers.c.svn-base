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

#include <hal/current/locore.h>
#include <hal/current/types.h>
#include <dev/drivers.h>
#include <init/std.h>


/* Device drivers dispatch table */
struct {
	fops_t *dispatch[DEV_MAX_MAJOR][DEV_MAX_MINOR];
	mutex_t mutex;
} drivers;


/* Function registers new device */
int drivers_register(uint_t major, uint_t minor, fops_t *fops)
{
	if (major > DEV_MAX_MAJOR)
		return -1;
	if (minor > DEV_MAX_MINOR)
		return -1;

	lock(&drivers.mutex);	
	drivers.dispatch[major][minor] = fops;
	unlock(&drivers.mutex);
	return 0;
}


/* Function initializes device driver interface */
void drivers_init(void)
{
	unlock(&drivers.mutex);
	return;
}


/*
 * Interface functions
 */


int dev_open(dev_t *dev)
{
	if (drivers.dispatch[dev->major][dev->minor] == NULL)
		return -1;
	
	return drivers.dispatch[dev->major][dev->minor]->open(dev);
}


int dev_read(dev_t *dev, char *buff, int *pos, int length)
{
	return drivers.dispatch[dev->major][dev->minor]->read(dev, buff, pos, length);
}


int dev_write(dev_t *dev, char *buff, int *pos, int length)
{
	return drivers.dispatch[dev->major][dev->minor]->write(dev, buff, pos, length);
}


int dev_ioctl(dev_t *dev, uint_t func, int *val)
{
	return drivers.dispatch[dev->major][dev->minor]->ioctl(dev, func, val);
}
