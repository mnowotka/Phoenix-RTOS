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

#ifndef _ARCHCONT_H_
#define _ARCHCONT_H_


#include <hal/current/types.h>
#include <hal/current/pmap.h>


#define ARCHCONT_SIZE      36


/* Architecture dependent part of CPU context */
typedef struct {
	uint_t esp0;       /* top of the kernel stack */
	uint_t cr3;        /* page directory register */
	void *kstack;      /* kstack starting address */
} archcont_t;


/* IA32 TSS (Task State Segment) */
typedef struct {
	ushort_t bl, _blh;
	uint_t esp0;
	ushort_t ss0, _ss0h;
	uint_t esp1;
	ushort_t ss1, _ss1h;
	uint_t esp2;
	ushort_t ss2, _ss2h;
	uint_t cr3;
	uint_t eip;
	uint_t eflags;
	uint_t eax, ecx, edx, ebx;
	uint_t esp;
	uint_t ebp;
	uint_t esi;
	uint_t edi;
	ushort_t es, _esh;
	ushort_t cs, _csh;
	ushort_t ss, _ssh;
	ushort_t ds, _dsh;
	ushort_t fs, _fsh;
	ushort_t gs, _gsh;
	ushort_t ldt, _ldth;
	ushort_t trfl, bitmap_addr;
} tss_t;


/*
 * Function initializes architecture dependent structures used
 * in CPU context switching. In IA32 architecture this is TSS
 * segment and TR register
 */
extern void archcont_init(void);


/*
 * Function creates architecture dependent CPU context. It's called by create_task() and
 * create_kernel_thread() routines
 */
extern void archcont_create(archcont_t *ac, uint_t start, uint_t kstack, uint_t stack, uint_t type, pmap_t *pmap);


/* Function returns starting address of kernel stack */
extern void *archcont_getkstack(archcont_t *ac);


#endif
