/*
 * Phoenix-RTOS
 *
 * Standard library
 *
 * Library interface
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

#ifndef _LIBPH_H_
#define _LIBPH_H_


/*
 * Basic types and constants
 */


#define NULL 0


typedef unsigned int uint_t;
typedef unsigned char uchar_t;
typedef volatile uint_t mutex_t;

typedef unsigned char u8;


/*
 * String manipulation
 */


extern uint_t ph_atoi(char *s);

extern uint_t ph_ahtoi(char *s);

extern int ph_strlen(char *s);

extern int ph_strncmp(char *s1, char *s2, uint_t count);

extern void ph_printf(char *fmt, ...);


/*
 * Kernel device drivers routines
 */


typedef struct _dev_t {
	uint_t major;
	uint_t minor;
} dev_t;


extern dev_t dev_console;
extern dev_t dev_serial0;
extern dev_t dev_serial1;
extern dev_t dev_serial2;
extern dev_t dev_serial3;


extern int ph_devwrite(dev_t *dev, char *buff, uint_t *pos, uint_t length);

extern int ph_devread(dev_t *dev, char *buff, uint_t *pos, uint_t length);


/*
 * System routines
 */


/* Task types */
#define KERNEL_TASK  0
#define USER_TASK    1


/* Task states */
#define TASK_RUNNING   0
#define TASK_SLEEPING  1
#define TASK_READY     2
#define TASK_STARTING  3

#define TASK_NAMESZ    32


typedef struct taskinfo {
	uint_t id;
	uint_t ppid;
	uint_t priority;
	uint_t cpu;
	uint_t state;
	uint_t type;
	char name[TASK_NAMESZ];
} taskinfo_t;


typedef struct _meminfo_t {
	uint_t total;
	uint_t total_free;
  uint_t kernel_rsvd;
	uint_t dma_free;
}	meminfo_t;


extern void ph_gettasks(uint_t *pids, uint_t length, uint_t *ntasks);

extern int ph_gettaskinfo(uint_t pid, taskinfo_t *ti);

extern void ph_getmeminfo(meminfo_t *mi);

extern int ph_raise(uint_t pid, uint_t sig);

extern int ph_exec(char *name);

extern uint_t ph_wait(int *err);

extern void ph_exit(int err);

extern int ph_sigset(uint_t sig, void (*handler)(void));

extern void ph_sleep(uint_t delay);

extern void ph_inject(void *addr, u8 mask, u8 op);


#endif
