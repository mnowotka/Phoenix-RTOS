/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Timer subsystem
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
#include <hal/current//defs.h>
#include <hal/current/interrupts.h>
#include <hal/current/timedev.h>
#include <init/std.h>
#include <vm/vm.h>
#include <task/task.h>
#include <task/scheduler.h>


/* Structure defining kernel timer. Used by all kinds of sleep functions */
typedef struct timer {
	volatile uint_t delay; /* delay time in milliseconds */
	struct timer *next;    /* next timer on the list */
	struct timer *prev;    /* previous timer on the list */
	uint_t *var;           /* monitored variable */
	uint_t val;            /* awaiting value */
	task_t *task;          /* awaiting task */
} timer_t;


struct {
	uint_t slice;     /* hardware timer tic */
	uint_t tics;      /* number of tics from the system start*/
	timer_t *tl;      /* timer list */
	mutex_t mutex;    /* access mutex */
} timesys;


void *time_intr_handler(uint_t intr)
{
	volatile timer_t *t;
	
	/* Inform interrupt controler about interrupt handling */
	__intr_end(intr);
	
	/* Interrupts are still blocked... */
	
	/* Interrupt cascading prevention */
	if (scheduler_depth() >= 1)
		return 0;
	
	lock(&timesys.mutex);		
	/* scheduler_lock(); */
	timesys.tics++;
	
	/* Browse timer list an wake up tasks (MOD) */
	for (t = timesys.tl; t != NULL; t = t->next) {
		if (t->var != NULL) {		
			if (*t->var != t->val) {
				t->task->state = TASK_READY;
				t->delay = 0;
			}			

			if (!t->delay) continue;
		}
					
		if (t->delay > 0) t->delay--;
		if (!t->delay) t->task->state = TASK_READY;	
	}
	
	/* scheduler_unlock(); */
	unlock(&timesys.mutex);
	
	return scheduler_schedule(intr);
}


/* Function initializes timer subsystem */
int timesys_init(uint_t slice)
{
	timesys.slice = slice;
	timesys.tics = 0;
	timesys.tl = NULL;
	unlock(&timesys.mutex);
	timedev_init(slice);
	
	set_intr_handler(0, &time_intr_handler); 
	
	return 0;
}


/* Macro adds timer to list */
#define timesys_add(t) {         \
	if (timesys.tl == NULL) {      \
		timesys.tl = &t;             \
		t.next = NULL;               \
		t.prev = NULL;               \
	}                              \
	else {                         \
		t.next = timesys.tl;         \
		timesys.tl->prev = &t;       \
		t.prev = NULL;               \
		timesys.tl = &t;             \
	}                              \
}


/* Macro removes timer from list */
#define timesys_remove(t) {                                            \
	lock_cli(&timesys.mutex);                                            \
	if (t.prev != NULL) t.prev->next = t.next; else timesys.tl = t.next; \
	if (t.next != NULL) t.next->prev = t.prev;                           \
	if ((t.prev == NULL) && (t.next == NULL)) timesys.tl = NULL;         \
	unlock_sti(&timesys.mutex);                                          \
}


/*
 * Function suspends task execution for time given by delay (in miliseconds).
 * Task sleeping can't be interrupted by signals.
 */
void sleep_unintr(uint_t delay)
{
	timer_t t;
			
	lock_cli(&timesys.mutex);	
	//scheduler_lock();
	t.delay = delay * 1000 / timesys.slice;
	t.task = __scheduler_getcurrent();
	
	t.var = NULL;
	timesys_add(t);
		
	//scheduler_unlock();
	unlock_sti(&timesys.mutex);
	
	for (;;) {
		t.task->state = TASK_SLEEPING;
		reschedule();
		
		if (!t.delay) break;
	}
	
	timesys_remove(t);
	return;
}


/*
 * Function suspends task execution while variable pointed by var isn't equal
 * to val or while the time doesn't expire. Sleep state can't be interrupted by
 * signals.
 */
void sleep_on_unintr(uint_t delay, uint_t *var, uint_t val)
{
	timer_t t;
			
	lock_cli(&timesys.mutex);	
	//scheduler_lock();
	
	t.delay = delay * 1000 / timesys.slice;
	t.task = __scheduler_getcurrent();	
	t.var = var;
	t.val = val;
	timesys_add(t);			

	//scheduler_unlock();
	unlock_sti(&timesys.mutex);

	for (;;) {
		t.task->state = TASK_SLEEPING;
		reschedule();
		if (!t.delay) break;
	}
	
	timesys_remove(t);

	return;
}


/*
 * Function suspends task execution while variable pointed by var isn't equal
 * to val or while time doesn't expire. Sleep state can be interrupted by the signal.
 */
void sleep_on(uint_t delay, uint_t *var, uint_t val)
{
	timer_t t;
			
	lock_cli(&timesys.mutex);	
	//scheduler_lock();
	
	t.delay = delay * 1000 / timesys.slice;
	t.task = __scheduler_getcurrent();	
	t.var = var;
	t.val = val;
	timesys_add(t);			

	//scheduler_unlock();
	unlock_sti(&timesys.mutex);

	t.task->state = TASK_SLEEPING;
	reschedule();	
	timesys_remove(t);

	return;
}
