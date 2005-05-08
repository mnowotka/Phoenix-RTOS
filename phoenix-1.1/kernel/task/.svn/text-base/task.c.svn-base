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

#include <hal/current/if.h>
#include <init/std.h>
#include <vm/vm.h>
#include <vm/kmalloc.h>
#include <task/task.h>
#include <task/scheduler.h>
#include <comm/signals.h>


/* Function creates kernel thread. When stack is NULL new page is allocated */
task_t *create_kernel_thread(char *name, void *start, void *stack, uchar_t type)
{
	task_t *task;
	void *kstack;
	unsigned int l;
		
	if ((task = (task_t *)kmalloc(sizeof(task_t))) == NULL)
		return NULL;
	
	l = min(std_strlen(name), TASK_NAME_SIZE);	
	task->name[l] = 0;
		
	task->type = KERNEL_TASK;
	task->ppid = 0;
	task->priority = 0;
	task->sigmap = 0;
	for (l = 0; l < sizeof(task->sigmap); l++)
		task->sighandlers[l] = 0;
	
	/* Allocate stack for new task */
	if (stack == NULL) {
		if ((kstack = (void *)kernel_pages_alloc(1)) == NULL) {
			return NULL;
		}
	}
	else
		kstack = stack;
	
	/* Create architecture dependent context */
	archcont_create(&task->ac, (uint_t)start, (uint_t)kstack, 0, KERNEL_TASK, NULL);
	
	/* Add to scheduler list */
	scheduler_addtask(task);
	
	return task;
}


/*
 * Function creates user task based on given vm_map structure. It allocates
 * memory for both stacks - kernel and user and creates the pmap structure for new
 * task. Next it creates task virtual space (maps all segments) and architecture
 * dependent CPU context.
 */
task_t *create_task(char *name, vm_map_t *map, void *start, uint_t priority)
{
	task_t *task, *current;
	void *kstack;
	page_t *ustack_area;
	vm_seg_t *sseg;
	uint_t l;
		
	/* Allocate new task structure */
	if ((task = (task_t *)kmalloc(sizeof(task_t))) == NULL)
		return NULL;
	
	/* Initialize important fields */	
	task->priority = priority;
	task->sigmap = 0;
	for (l = 0; l < sizeof(task->sigmap); l++)
		task->sighandlers[l] = 0;
	
	task->vm_map = map;
	
	/* Set relationships beetwen tasks */
	if ((current = scheduler_getcurrent()) != NULL)
		task->ppid = current->id;
	else
		task->ppid = NULL;
	
	if ((l = std_strlen(name)) > TASK_NAME_SIZE) {
		memcpy(task->name, name, TASK_NAME_SIZE);
		task->name[TASK_NAME_SIZE] = 0;
	}
	else
		memcpy(task->name, name, l + 1);
		
	task->type = USER_TASK;
	
	/* Allocate one page kernel stack for new task */
	if ((kstack = (void *)kernel_pages_alloc(1)) == NULL)
		return NULL;

	/* Allocate four page user stack for new task */
	if ((ustack_area = (void *)area_alloc(STACK_SIZE, REG_MEM)) == NULL)
		return NULL;

	sseg = seg_create(ustack_area,
	                  (void *)USER_STACK_TOP - STACK_SIZE * PAGE_SIZE,
	                  PGHD_PRESENT | PGHD_WRITE | PGHD_READ | PGHD_NOEXEC | PGHD_USER);
	
	if (!sseg)
		return NULL;
	if (seg_map(map, sseg) < 0)
		return NULL;
		
	/* Create architecture dependent CPU context */
	archcont_create(&task->ac, (uint_t)start, (uint_t)kstack, (uint_t)sseg->vaddr, USER_TASK, map->pmap);

	scheduler_addtask(task);
	return task;
}


/*
 * Function stops current user task. Kernel thread can't be stopped.
 * When task is terminated by this function, kernel releases all segments
 * and sends notification signal to parent task to release child task structure,
 * pmap interface and kernel stack. When task is orphanned (parent task doesn't exist)
 * notification is sent to taks_run
 */
void exit_task(int err)
{
	task_t *task;
	
	/* Obtain current task structure */
	task = scheduler_getcurrent();
	
	/* If task is user task release its segments and memory map */
	if (task->type == KERNEL_TASK)
		return;

	/* Release all task segments and memory map */
	release_segs(task->vm_map);
	task->exit = err;
	
	/*
	 * Send exit notification to parent task. If taks is orphan send notification
	 * to task_run, which must exist.
	 */
	if (raise(task->ppid, SIGCHLD) < 0) {
		task->ppid = 1;
		raise(task->ppid, SIGCHLD);
	}
	
	task->state = TASK_ZOMBIE;
	reschedule();
	
	return;
}


/* exit (PSC) */
void psc_exit(int err)
{
	exit_task(err);
	return;
}


/* Function suspends task execution and waits for SIGCHLD signal */
uint_t wait(int *err)
{
	task_t *task;
	
	if ((task = scheduler_getcurrent()) == NULL)
		return 0;
	
	task->chldpid = 0;
	while (!task->chldpid) {
	
		task->state = TASK_CHLDWAITING;	
		reschedule();		
	}

	*err = task->chldexit;	
	return task->chldpid;
}


/* wait (PSC) */
void psc_wait(uint_t *pid, int *err)
{
	*pid = wait(err);
	return;
}
