/*
 * Phoenix-RTOS
 *
 * Standard library
 *
 * System routines
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
#include <stdarg.h>
#include <arch/locore.h>


void ph_gettasks(uint_t *pids, uint_t length, uint_t *ntasks)
{
	__gettasks(pids, length, ntasks);	
	return;
}


int ph_gettaskinfo(uint_t pid, taskinfo_t *ti)
{
	return __gettaskinfo(pid, ti);
}


void ph_getmeminfo(meminfo_t *mi)
{
	__getmeminfo(mi);
	return;
}


int ph_raise(uint_t pid, uint_t sig)
{
	return __raise(pid, sig);
}


int ph_exec(char *name)
{
	return __exec(name);
}


uint_t ph_wait(int *err)
{
	return __wait(err);
}


void ph_exit(int err)
{
	__exit(err);
	for (;;);
}


int ph_sigset(uint_t sig, void (*handler)(void))
{
	return __sigset(sig, handler);
}


void ph_sleep(uint_t delay)
{
	return __sleep_unintr(delay);
}


void ph_inject(void *addr, u8 mask, u8 op)
{
	return __hal_inject(addr, mask, op);
}
