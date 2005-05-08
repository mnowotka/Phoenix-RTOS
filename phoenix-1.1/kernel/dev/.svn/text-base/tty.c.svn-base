/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Console driver
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
#include <hal/current/interrupts.h>
#include <hal/current/types.h>
#include <hal/current/console.h>
#include <init/std.h>
#include <vm/vm.h>
#include <vm/kmalloc.h>
#include <task/timesys.h>
#include <dev/drivers.h>
#include <dev/tty.h>


#define TTY_BUFF_SIZE      128


/* tty descriptor */
struct _tty_data {
	uint_t op;                                     /* typ aktualnie wykonywanej operacji */
	volatile uint_t rpos;                          /* pozycja ostatnio odczytanego znaku */
	volatile uint_t wpos;                          /* pozycja ostatnio zapisanego znaku przez isr */
	volatile uint_t isempty;                       /* emptyness flag */
	char rbuff[TTY_BUFF_SIZE];                     /* bufor odbiorczy */
	mutex_t mutex;                                 /* zamek dostepowy */	
} ttyd;


/* Device interface for tty */
int tty_open(dev_t *dev);
int tty_write(dev_t *dev, char *buff, uint_t *pos, uint_t size);
int tty_read(dev_t *dev, char *buff, uint_t *pos, uint_t length);
int tty_ioctl(dev_t *dev, uint_t mode, int *val);
void tty_close(dev_t *dev);

fops_t tty_fops = {
	tty_open,
	tty_write,
	tty_read,
	tty_ioctl,
	NULL,
	tty_close
};


void keyb_intr(uint_t intr)
{
	char *s;
	__intr_end(intr);
	
	lock(&ttyd.mutex);	
	s = keyb_get();
	
	if (*s) {
		ttyd.rbuff[ttyd.wpos] = *s;
		ttyd.wpos = (++ttyd.wpos % TTY_BUFF_SIZE);
		ttyd.isempty = 0;
	}
	unlock(&ttyd.mutex);
	
	return;
}


/* Function initializes tty console driver */
void tty_init(void)
{	
	ttyd.wpos = 0;
	ttyd.rpos = 0;
	ttyd.isempty = 1;
	unlock(&ttyd.mutex);
	
	/* Set intr handler */
	set_intr_handler(CONSOLE_INTR, (void *)&keyb_intr);
	
	/* Register driver */
	if (drivers_register(TTY_MAJOR, 0, &tty_fops) < 0) {
		std_printf("tty: Can't register device at %d, %d!\n", TTY_MAJOR, 0);
		return;
	}
	return;
}


int tty_open(dev_t *dev)
{
	return 0;
}


int tty_write(dev_t *dev, char *buff, uint_t *pos, uint_t length)
{
	uint_t count = length;
	
	/* VULN, MOD */
	console_lock();
	console_puts(0, buff);
	console_unlock();
	
	return count;
}


int tty_read(dev_t *dev, char *buff, uint_t *pos, uint_t length)
{
	uint_t count;
	
	/* Wait for characters from the keyboard */
	sleep_on(0, &ttyd.isempty, 1);
	
	lock_cli(&ttyd.mutex);
	if (ttyd.wpos > ttyd.rpos)
		count = min(ttyd.wpos - ttyd.rpos, length);
	else
		count = min(TTY_BUFF_SIZE - ttyd.rpos, length);
		
	/* Set emptyness flag */
	if (ttyd.rpos + count >= ttyd.wpos)
		ttyd.isempty = 1;
	
	memcpy(buff, &ttyd.rbuff[ttyd.rpos], count);
	ttyd.rpos += count;
	
	if (ttyd.rpos == TTY_BUFF_SIZE)
		ttyd.rpos = 0;
	unlock_sti(&ttyd. mutex);
	
	return count;
}


int tty_ioctl(dev_t *dev, uint_t mode, int *val)
{
	return 0;
}


void tty_close(dev_t *dev)
{
	return;
}
