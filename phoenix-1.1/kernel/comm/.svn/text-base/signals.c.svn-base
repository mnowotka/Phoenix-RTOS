/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Signals
 *
 * Copyright 2001, 2005 Pawel Pisarczyk
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
#include <task/scheduler.h>
#include <comm/signals.h>


void do_kill(void);
void do_releasechild(task_t *task);


#define CLRSIG(task, k)        (task->sigmap &= ~(0x80000000 >> k))


/*
 * Local signal handler. Function is called when interrupts on current CPU
 * are blocked. Scheduler requests can't be cascaded!
 */ 
void *handle_local_signals(uint_t pc)
{
	uint_t k;
	task_t *task;
		
	/* Obtain current task description */
	if ((task = __scheduler_getcurrent()) == NULL)
		return NULL;
		
	for (k = 0; k < NSIGNALS; k++) {
	
		/*
		 * Test if new signal is received, push received signal number and return address on
		 * user stack and modify return address from the interrupt to the handler address.
		 */
		 if (task->sigmap >> (31 - k)) {			
			switch (k) {
			case SIGKILL:
				CLRSIG(task, k);
				if (task->type != KERNEL_TASK) do_kill();	
				return NULL;
			case SIGCHLD:
				CLRSIG(task, k);
				do_releasechild(task);	
				return NULL;
			}
					
			/* Call user handler only when returns to user mode */
			if ((task->type != KERNEL_TASK) && (pc >= KERNEL_BASE))
				return NULL;
			CLRSIG(task, k);			
			return task->sighandlers[k];
		}
	}
	return NULL;
}


void do_kill(void)
{
	exit_task(-1);
	return;
}


/* Function releases rest of child structures */
void do_releasechild(task_t *task)
{
	task_t *child;

	/* Find child which has been terminated */
	if ((child = scheduler_findchild(task->id)) == NULL)
		return;
	
	/*
	 * Get an errorcode, store it in parent task structure, remove rest
	 * of the child task structures and remove task from scheduler queue.
	 */	
	task->chldexit = child->exit;
	task->chldpid = child->id;
	
	_scheduler_removetask(child);
	kernel_pages_free(archcont_getkstack(&child->ac));
	
	pmap_free(child->vm_map->pmap);
	kfree(child->vm_map);
	kfree(child);
	
	return;
}	


/* Function installs user level signal handler for current task */
int sigset(uint_t sig, void (*handler)(void))
{
	task_t *task;
		
	if ((task = scheduler_getcurrent()) == NULL)
		return -1;
		
	if (sig >= 32) return -1;
	
	task->sighandlers[sig] = handler;
	return 0;
}


/* sigset (PSC) */
void psc_sigset(uint_t sig, void (*handler)(void), int *err)
{
	*err = sigset(sig, handler);
	
	return;
}
