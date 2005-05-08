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

#include <hal/current/if.h>
#include <init/std.h>
#include <vm/vm.h>
#include <vm/kmalloc.h>
#include <task/task.h>


/* Scheduler queue */
struct {
	mutex_t mutex;
	uint_t ntasks;    /* number of tasks in queue */
	task_t *tasks;    /* task scheduling list */
	task_t *current;  /* current */
	uint_t depth;     /* interrupt depth - field used to prevent interrupt cascading */
	uint_t lastid;    /* last allocated identifier */
} scheduler;


/* Function locks scheduler */
void scheduler_lock(void)
{
	lock(&scheduler.mutex);
	return;
}


/* Function unlocks scheduler */
void scheduler_unlock(void)
{
	unlock(&scheduler.mutex);
	return;
}



task_t *__scheduler_getcurrent(void)
{
	task_t *current;

	current = scheduler.current;
	return current;
}


/* Function returns pointer to current task (with locking) */
task_t *scheduler_getcurrent(void)
{
	task_t *current;

	lock_cli(&scheduler.mutex);
	current = scheduler.current;
	unlock_sti(&scheduler.mutex);	
	return current;
}


/*
 * Function returns scheduler depth. When scheduler is called depth is increased,
 * when leaved depth is decreased. This functions is very important because
 * during scheduling function execution interrupts are unmasked. In some cases,
 * when scheduling duration time is to long, interrupts are cascaded
 * and scheduler depth informs about this fact and prevents deadlock.
 */
uint_t scheduler_depth(void)
{
	return scheduler.depth;
}


/* Function returns current task id */
int scheduler_getpid(void)
{
	task_t *task;
	
	if ((task = scheduler_getcurrent()) == NULL)
		return -1;
	else
		return (task->id);
}


/* Function returns number of tasks in scheduler queue */
uint_t scheduler_getntasks(void)
{
	uint_t ntasks;
	
	lock_cli(&scheduler.mutex);
	ntasks = scheduler.ntasks;
	unlock_sti(&scheduler.mutex);	
	return ntasks;
}


/* Function adds task to queue */
void scheduler_addtask(task_t *task)
{
	lock(&scheduler.mutex);
	
	if (scheduler.tasks == NULL) {
		scheduler.tasks = task;
		task->next = task;
		task->prev = task;	
	}
	else {
		scheduler.tasks->prev->next = task;
		task->next = scheduler.tasks;
		task->prev = scheduler.tasks->prev;
		scheduler.tasks->prev = task;
		scheduler.tasks = task;		
	}
	scheduler.ntasks++;
	task->state = TASK_STARTING;
	
	/* (MOD) */
	task->id = ++scheduler.lastid;
	
	unlock(&scheduler.mutex);
		
	return;
}


/* Function removes current task from scheduler queue */
void _scheduler_removetask(task_t *task)
{
	lock(&scheduler.mutex);
	
	if (task->next == task) {
		std_printf("KERNEL PANIC: No tasks in the system!\n");
		unlock(&scheduler.mutex);
		return;
	}
	
	scheduler.ntasks--;		
	task->prev->next = task->next;
	task->next->prev = task->prev;
	
	if (task == scheduler.tasks)
		scheduler.tasks = task->next;

	unlock(&scheduler.mutex);
	return;
}


/* Scheduler routine (simple Round-Robin) */
void *scheduler_schedule(uint_t intr)
{
	task_t *task, *old;
	uint_t cnt = 0;
			
	scheduler.depth++;

	lock(&scheduler.mutex);
			
	sti();
	
	/* Select next task. */
	if (!scheduler.current)
		task = scheduler.tasks;
	else
		task = scheduler.current;

	/* If no other tasks available return to current task */
	if (!task) {
		unlock(&scheduler.mutex);
		cli();
		scheduler.depth--;
		return 0;
	}
			
	for (;;) {
		if ((task->state != TASK_READY) && (task->state != TASK_STARTING)) {
			task = task->next;

			/* If no other ready to run tasks available return to current task */
			if (++cnt > scheduler.ntasks) {
				unlock(&scheduler.mutex);				
				cli();
				scheduler.depth--;
				return task;
			}
			continue;	
		}
		
		if ((scheduler.current) && (scheduler.current->state == TASK_RUNNING))
			scheduler.current->state = TASK_READY;
		
		old = scheduler.current;
		scheduler.current = task;
		
		if (task->state == TASK_STARTING) {
			task->state = TASK_RUNNING;
			unlock(&scheduler.mutex);
			
			cli();			
			scheduler.depth--;
			
			/* Start task - only start context is available on kernel stack */
			switch_to(old, task);
		}
		else {			
			task->state = TASK_RUNNING;
			unlock(&scheduler.mutex);

			cli();			
			scheduler.depth--;
			
			/* Change context - full frame is available */
			switch_context (old, task);
			break;
		}		
	}
		
	return task;
}


/* Function initializes scheduler */
int scheduler_init(void)
{
	/* Initialize architecture dependent structures */
	archcont_init();
	
	/* Initialize scheduler queue */
	unlock(&scheduler.mutex);
	scheduler.ntasks = 0;
	scheduler.tasks = NULL;
	scheduler.current = NULL;
	scheduler.depth = 0;
	scheduler.lastid = 0;         /* MOD */

	return 0;
}


/* Functions returns array of running task pids (PSC) */
int scheduler_gettasks(uint_t pids[], uint_t length, uint_t *ntasks)
{
	task_t *task, *etask;
	uint_t n = 0;

	/* Lock task queue */
	lock_cli(&scheduler.mutex);	
	task = scheduler.tasks;

	if (!task) {
		*ntasks = 0;
		unlock_sti(&scheduler.mutex);
		return -1;
	}
			
	etask = task;
	do {
		pids[n++] = task->id;
		task = task->next;
	} while (task != etask);
	
	*ntasks = n;
	unlock_sti(&scheduler.mutex);
	return 0;
}


/* Function returns task structure for task given by pid (PSC) */
int scheduler_gettaskinfo(uint_t pid, task_t *ti, int *err)
{
	task_t *task, *etask;

	/* Lock task queue */
	lock_cli(&scheduler.mutex);	
	task = scheduler.tasks;

	if (!task) {
		*err = -1;
		unlock_sti(&scheduler.mutex);
		return -1;
	}
			
	etask = task;
	do {
		if (task->id == pid) {
			memcpy(ti, task, TASK_INFO_SIZE);         /* copy only first fields */
			*err = 0;
			unlock_sti(&scheduler.mutex);
			return 0;
		}
		task = task->next;
	} while (task != etask);
	
	*err = -1;
	unlock_sti(&scheduler.mutex);
	return 0;
}


/* Function sends signal to task given by pid */
int raise(uint_t pid, uint_t sig)
{
	task_t *task, *etask;
	
	/* Lock task queue */
	lock_cli(&scheduler.mutex);	
	task = scheduler.tasks;

	if (!task) {
		unlock_sti(&scheduler.mutex);
		return -1;
	}
			
	etask = task;
	do {
		if (task->id == pid) {
			task->sigmap |= (0x80000000 >> sig);
			task->state = TASK_READY;
			unlock_sti(&scheduler.mutex);
			return 0;
		}
		task = task->next;
	} while (task != etask);
	
	unlock_sti(&scheduler.mutex);
	return -1;
}


/* raise() (PSC) */
void psc_raise(uint_t pid, uint_t sig, int *err)
{
	*err = raise(pid, sig);
	return;
}


/* Function finds zombie child for task given by pid */
task_t *scheduler_findchild(uint_t pid)
{
	task_t *task, *etask;
	
	/* Lock task queue */
	lock(&scheduler.mutex);	
	task = scheduler.tasks;

	if (!task) {
		unlock_sti(&scheduler.mutex);
		return NULL;
	}
				
	etask = task;
	do {
		if ((task->ppid == pid) && (task->state == TASK_ZOMBIE)) {
			unlock(&scheduler.mutex);
			return task;
		}
		task = task->next;
	} while (task != etask);
	
	unlock(&scheduler.mutex);
	return NULL;
}
