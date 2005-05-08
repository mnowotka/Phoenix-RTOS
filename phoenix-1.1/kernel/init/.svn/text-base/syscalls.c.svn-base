/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * System calls
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

#include <hal/current/if.h>
#include <dev/drivers.h>
#include <task/if.h>
#include <comm/signals.h>
#include <init/ramdisk.h>


#define SYSCALL(p) ((void *)p)


/*
 * structure describing system calls
 */

typedef struct _sysent_t {
	void *syscall;    /* function pointer */
	char *name;       /* function name */
	uint_t args;      /* arguments counter */
	uint_t padd;      /* padding */
} sysent_t;


sysent_t sysents[] = {
	{ SYSCALL(&dev_open), "dev_open", 3, 0 },           /* 0 */
	{ SYSCALL(&dev_write), "dev_write", 3, 0 },
	{ SYSCALL(&dev_read), "dev_read", 3, 0 },
	{ SYSCALL(&dev_ioctl), "dev_ioctl", 3, 0 },
	{ SYSCALL(&console_puts), "console_puts", 2, 0 },           /* 4 */
	{ SYSCALL(&console_puts), "console_puts", 2, 0 },
	{ SYSCALL(&scheduler_gettasks), "gettasks",    3, 0 },
	{ SYSCALL(&scheduler_gettaskinfo), "gettaskinfo", 3, 0 },
	{ SYSCALL(&get_meminfo), "getmeminfo", 1, 0 },           /* 8 */
	{ SYSCALL(&psc_raise), "raise", 3, 0 },
	{ SYSCALL(&psc_exec), "exec", 2, 0 },
	{ SYSCALL(&psc_wait), "wait", 2, 0 },
	{ SYSCALL(&psc_exit), "exit", 1, 0 },           /* 12 */
	{ SYSCALL(&psc_sigset), "sigset", 3, 0 },
	{ SYSCALL(&sleep_unintr), "sleep_unintr", 1, 0 },
	{ SYSCALL(&get_ramdisk_info), "get_ramdisk_info", 2, 0 },
	{ SYSCALL(hal_inject), "hal_inject", 3, 0 }
};
