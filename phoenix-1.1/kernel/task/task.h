/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Task manager
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

#ifndef _TASK_H_
#define _TASK_H_

#include <hal/current/types.h>
#include <hal/current/archcont.h>
#include <vm/vm.h>
#include <dev/drivers.h>


#define TASK_NAME_SIZE   36
#define TASK_INFO_SIZE   TASK_NAME_SIZE + 20


/* Taks types */
#define KERNEL_TASK  0
#define USER_TASK    1


/* Task states */
#define TASK_RUNNING      0  /* task is running on some CPU */
#define TASK_SLEEPING     1  /* task is sleeping */
#define TASK_READY        2  /* task is ready to run */
#define TASK_STARTING     3  /* task is starting - on the stack exist only initial context */
#define TASK_CHLDWAITING  4  /* task waits for child exit */
#define TASK_ZOMBIE       5  /* taks exits but parent task isn't notified about this fact yet */


/* Initial stack size in pages - on IA32 16KB */
#define STACK_SIZE  4


/* Task stucture */
typedef struct task {
	uint_t id;                   /* task identifier */
	uint_t ppid;                 /* parent task identifier */
	uint_t priority;             /* task priority, used by scheduler */
	uint_t cpu;                  /* associated CPU */
	volatile uint_t state;       /* task state */
	uint_t type;                 /* task type - kernel or user */
	char name[TASK_NAME_SIZE];   /* task name */
	struct task *next;           /* next task on list */
	struct task *prev;           /* previous task on list */
	archcont_t ac;               /* architecture dependent context, registers, kernel stack etc. */
	vm_map_t *vm_map;            /* virtual memory map - used by user tasks */
	volatile uint_t sigmap;      /* received signals map - 32 items */
	void *sighandlers[32];       /* signal handlers */
	dev_t *tty;                  /* associated tty device */
	int exit;                    /* exit code */
	volatile int chldexit;       /* child exit code, used by wait() function */
	volatile uint_t chldpid;     /* stopped child pid, used by wait() function */
} task_t;


/* Function creates kernel thread. When stack is NULL new page is allocated */
extern task_t *create_kernel_thread(char *name, void *start, void *stack, uchar_t type);


/*
 * Function creates user task based on given vm_map structure. It allocates
 * memory for both stacks - kernel and user and creates a pmap structure for new
 * task. Next it creates task virtual space (maps all segments) and architecture
 * dependent CPU context.
 */
extern task_t *create_task(char *name, vm_map_t *map, void *start, uint_t priority);


/*
 * Function stops current user task. Kernel thread can't be stopped.
 * When task is terminated by this function, kernel releases all segments
 * and sends notification signal to parent task to release child task structure,
 * pmap interface and kernel stack. When task is orphanned (parent task doesn't exist)
 * notification is sent to taks_run
 */
extern void exit_task(int err);


/* exit (PSC) */
extern void psc_exit(int err);


/* Function suspends execution and waits for SIGCHLD signal */
extern uint_t wait(int *err);


/* wait (PSC) */
extern void psc_wait(uint_t *pid, int *err);


#endif
