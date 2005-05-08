/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Task scheduler
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

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <hal/current/types.h>
#include <task/task.h>


/* Function locks scheduler */
extern void scheduler_lock(void);


/* Function unlocks scheduler */
extern void scheduler_unlock(void);


/* Function returns pointer to current task (without locking) */
extern task_t *__scheduler_getcurrent(void);


/* Function returns pointer to current task (with locking) */
extern task_t *scheduler_getcurrent(void);


/*
 * Function returns scheduler depth. When scheduler is called depth is increased,
 * when leaved depth is decreased. This functions is very important because
 * during scheduling function execution interrupts are unmasked. In some cases,
 * when scheduling duration time is to long, interrupts are cascaded
 * and scheduler depth informs about this fact and prevents deadlock.
 */
extern uint_t scheduler_depth(void);


/* Function returns current task id */
extern int scheduler_getpid(void);


/* Function returns number of tasks in scheduler queue */
extern uint_t scheduler_getntasks(void);


/* Function adds task to queue */
extern void scheduler_addtask(task_t *task);


/* Function removes current task from scheduler queue */
extern void _scheduler_removetask(task_t *task);


/* Scheduler routine (simple Round-Robin) */
extern void *scheduler_schedule(uint_t intr);


/* Function initializes scheduler */
extern int scheduler_init(void);


/* Functions returns array of running task pids (PSC) */
extern int scheduler_gettasks(uint_t pids[], uint_t length, uint_t *ntasks);


/* Function returns task structure for task given by pid (PSC) */
extern int scheduler_gettaskinfo(uint_t pid, task_t *ti, int *err);


/* Function sends signal to task given by pid */
extern int raise(uint_t pid, uint_t sig);


/* raise() (PSC) */
extern void psc_raise(uint_t pid, uint_t sig, int *err);


/* Function finds zombie child for task given by pid */
extern task_t *scheduler_findchild(uint_t pid);


#endif
