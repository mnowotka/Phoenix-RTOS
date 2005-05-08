/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Architecture dependent structures and functions used in CPU context switching
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

#include <hal/current/defs.h>
#include <hal/current/types.h>
#include <hal/current/archcont.h>
#include <hal/current/locore.h>
#include <hal/current/pmap.h>
#include <task/task.h>


/*
 * Macros used to operate on CPU context structure
 */
#define INT_VAL(esp, offs) (*(uint_t *)((void *)esp + offs))
#define SHORT_VAL(esp, offs) (*(short *)((void *)esp + offs))

#define CONT_EDI(esp)     INT_VAL(esp, 0)
#define CONT_ESI(esp)     INT_VAL(esp, 4)
#define CONT_EBP(esp)     INT_VAL(esp, 8)
#define CONT_EDX(esp)     INT_VAL(esp, 12)
#define CONT_ECX(esp)     INT_VAL(esp, 16)
#define CONT_EBX(esp)     INT_VAL(esp, 20)
#define CONT_EAX(esp)     INT_VAL(esp, 24)
#define CONT_GS(esp)      SHORT_VAL(esp, 28)
#define CONT_FS(esp)      SHORT_VAL(esp, 30)
#define CONT_ES(esp)      SHORT_VAL(esp, 32)
#define CONT_DS(esp)      SHORT_VAL(esp, 34)
#define CONT_EIP(esp)     INT_VAL(esp, 36)
#define CONT_CS(esp)      INT_VAL(esp, 40)
#define CONT_EFLAGS(esp)  INT_VAL(esp, 44)
#define CONT_ESP(esp)     INT_VAL(esp, 48)
#define CONT_SS(esp)      INT_VAL(esp, 52)


/*
 * Function initializes architecture dependent structures used
 * in CPU context switching. In IA32 architecture this is TSS
 * segment and TR register
 */
void archcont_init(void)
{
	memclr(&common_tss, sizeof(tss_t));
	common_tss.ss0 = KERNEL_DS;

	/* Create user level memory descriptors for user tasks */
	insert_gdtdesc(3, 0, 0xffffffff, UCODE_DESC);
	insert_gdtdesc(4, 0, 0xffffffff, UDATA_DESC);
	insert_gdtdesc(5, (uint_t)&common_tss, sizeof(tss_t), TSS_DESC);
	
	/* Set task register */
	settr(5 * 8);
	
	return;
}


/*
 * Function creates architecture dependent CPU context. It's called by create_task() and
 * create_kernel_thread() routines
 */
void archcont_create(archcont_t *ac, uint_t start, uint_t kstack, uint_t stack, uint_t type, pmap_t *pmap)
{	
	switch (type) {	
	case KERNEL_TASK:
		ac->cr3 = KERNEL_PAGE_DIR;
		ac->esp0 = kstack + PAGE_SIZE - ARCHCONT_SIZE - 12 - 4;	
		
		/* Prepare startup context for kernel thread */
		CONT_EDI(ac->esp0) = 0;
		CONT_ESI(ac->esp0) = 0;
		CONT_EBP(ac->esp0) = 0;
		CONT_EDX(ac->esp0) = 0;
		CONT_ECX(ac->esp0) = 0;
		CONT_EBX(ac->esp0) = 0;
		CONT_EAX(ac->esp0) = 0;
		
		CONT_GS(ac->esp0) = KERNEL_DS;
		CONT_FS(ac->esp0) = KERNEL_DS;
		CONT_ES(ac->esp0) = KERNEL_DS;
		CONT_DS(ac->esp0) = KERNEL_DS;
		
		CONT_EIP(ac->esp0) = start;
		CONT_CS(ac->esp0) = KERNEL_CS;	
		CONT_EFLAGS(ac->esp0) = get_eflags();
		break;
		
	case USER_TASK:
		if (pmap == NULL)
			ac->cr3 = KERNEL_PAGE_DIR;
		else
			ac->cr3 = (uint_t)KERNEL_TO_PHYS(pmap->pdir);
		
		ac->esp0 = kstack + PAGE_SIZE - ARCHCONT_SIZE - 12 - 4 - 8;
		
		/*
		 * Prepare startup context for user task. Startup context for user task is
		 * more complex than kernel thread context
		 */
		CONT_EDI(ac->esp0) = 0;
		CONT_ESI(ac->esp0) = 0;
		CONT_EBP(ac->esp0) = 0;
		CONT_EDX(ac->esp0) = 0;
		CONT_ECX(ac->esp0) = 0;
		CONT_EBX(ac->esp0) = 0;
		CONT_EAX(ac->esp0) = 0;
		
		CONT_GS(ac->esp0) = USER_DS;
		CONT_FS(ac->esp0) = USER_DS;
		CONT_ES(ac->esp0) = USER_DS;
		CONT_DS(ac->esp0) = USER_DS;
		
		CONT_EIP(ac->esp0) = start;
		CONT_CS(ac->esp0) = USER_CS;	
		CONT_EFLAGS(ac->esp0) = get_eflags();
		CONT_ESP(ac->esp0) = stack + STACK_SIZE * PAGE_SIZE - 4;
		CONT_SS(ac->esp0) = USER_DS;
		break;
	}
	
	return;
}


/* Function returns starting address of kernel stack */
void *archcont_getkstack(archcont_t *ac)
{
	return ac->kstack;
}
