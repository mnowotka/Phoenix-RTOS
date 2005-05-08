/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Hardware abstraction routines
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

#ifndef _LOCORE_H
#define _LOCORE_H

#ifndef __ASSEMBLY__

#include <hal/current/types.h>
#include <hal/current/defs.h>
#include <hal/current/archcont.h>
#include <task/task.h>


/* Global Task State Segment */
tss_t common_tss; 


static inline void memcpy(void *to, void *from, uint_t n)
{
   __asm__ volatile
	("cld; \
		movl %0, %%ecx; \
		movl %%ecx, %%edx; \
		andl $3, %%edx; \
		shrl $2, %%ecx; \
		movl %1, %%edi; \
		movl %2, %%esi; \
		rep; movsl; \
		movl %%edx, %%ecx; \
		rep; movsb"
	:
	: "g" (n), "m" (to), "m" (from)
	: "ecx", "edx", "esi", "edi");
}


static inline void memclr(void *where, uint_t n)
{
   __asm__ volatile
	("cld; \
		xorl	%%eax, %%eax; \
		movl	%0, %%edi; \
		movl  %1, %%ecx; \
		rep; stosb"
	:
	: "m" (where), "d" (n)
	: "eax", "ecx", "edi");
}


static inline void lock(mutex_t *mutex)
{
   __asm__ volatile
	(" \
	1: \
		xorl %%eax, %%eax; \
		xchgl %0, %%eax; \
		cmp $0, %%eax; \
		jz 1b"
	:
	: "m" (*mutex)
	: "eax");
}


static inline void unlock(mutex_t *mutex)
{
   __asm__ volatile
	(" \
		xorl %%eax, %%eax; \
		incl %%eax; \
		xchgl %0, %%eax"
	:
	: "m" (*mutex)
	: "eax");
}


static inline void sti(void)
{
	 __asm__ volatile ("sti":);
}


static inline void cli(void)
{
	 __asm__ volatile ("cli":);
}


static inline uint_t get_pc(void)
{
	uint_t pc;
	
	 __asm__ volatile
	(" \
		call 1f; \
	1: \
		popl %%eax; \
		movl %%eax, %0"
	:"=d" (pc)
	:
	:"eax");

	return pc;
}


static inline uint_t get_esp(void)
{
	uint_t esp;
	
	 __asm__ volatile ("movl %%esp, %0":"=d" (esp):);

	return esp;
}


static inline uint_t get_eax(void)
{
	uint_t r;
	
	 __asm__ volatile ("movl %%eax, %0":"=g" (r):);

	return r;
}


static inline uint_t get_ebx(void)
{
	uint_t r;
	
	 __asm__ volatile ("movl %%ebx, %0":"=g" (r):);

	return r;
}


static inline uint_t get_ecx(void)
{
	uint_t r;
	
	 __asm__ volatile ("movl %%ecx, %0":"=g" (r):);

	return r;
}


static inline uint_t get_edx(void)
{
	uint_t r;
	
	 __asm__ volatile ("movl %%edx, %0":"=g" (r):);

	return r;
}


static inline uint_t get_ebp(void)
{
	uint_t r;
	
	 __asm__ volatile ("movl %%ebp, %0":"=g" (r):);

	return r;
}


static inline uint_t get_esi(void)
{
	uint_t r;
	
	 __asm__ volatile ("movl %%esi, %0":"=g" (r):);

	return r;
}


static inline uint_t get_edi(void)
{
	uint_t r;
	
	 __asm__ volatile ("movl %%edi, %0":"=g" (r):);

	return r;
}


static inline uint_t get_eflags(void)
{
	uint_t eflags;
	
	__asm__ volatile
	(" \
		pushfl; \
		popl %%eax; \
		movl %%eax, %0"
	:"=d" (eflags)
	:
	:"eax");
		
	return eflags;
}


static inline uint_t get_cs(void)
{
	uint_t cs;
	
	__asm__ volatile
	(" \
		xorl %%eax, %%eax; \
		movw %%cs, %%ax; \
		movl %%eax, %0"
	:"=d" (cs)
	:
	:"eax");
		
	return cs;
}


static inline uint_t get_ds(void)
{
	uint_t ds;
	
	__asm__ volatile
	(" \
		xorl %%eax, %%eax; \
		movw %%ds, %%ax; \
		movl %%eax, %0"
	:"=d" (ds)
	:
	:"eax");
		
	return ds;
}


static inline uint_t get_es(void)
{
	uint_t es;
	
	__asm__ volatile
	(" \
		xorl %%eax, %%eax; \
		movw %%es, %%ax; \
		movl %%eax, %0"
	:"=d" (es)
	:
	:"eax");
		
	return es;
}


static inline uint_t get_fs(void)
{
	uint_t fs;
	
	__asm__ volatile
	(" \
		xorl %%eax, %%eax; \
		movw %%fs, %%ax; \
		movl %%eax, %0"
	:"=d" (fs)
	:
	:"eax");
		
	return fs;
}


static inline uint_t get_gs(void)
{
	uint_t gs;
	
	__asm__ volatile
	(" \
		xorl %%eax, %%eax; \
		movw %%gs, %%ax; \
		movl %%eax, %0"
	:"=d" (gs)
	:
	:"eax");
		
	return gs;
}


static inline uint_t get_ss(void)
{
	uint_t ss;
	
	__asm__ volatile
	(" \
		xorl %%eax, %%eax; \
		movw %%ss, %%ax; \
		movl %%eax, %0"
	:"=d" (ss)
	:
	:"eax");
		
	return ss;
}


static inline uint_t get_cr0(void)
{
	uint_t cr0;
	
	__asm__ volatile
	(" \
		movl %%cr0, %%eax; \
		movl %%eax, %0"
	:"=g" (cr0)
	:
	:"eax");
		
	return cr0;
}


static inline uint_t get_cr2(void)
{
	uint_t cr2;
	
	__asm__ volatile
	(" \
		movl %%cr2, %%eax; \
		movl %%eax, %0"
	:"=g" (cr2)
	:
	:"eax");
		
	return cr2;
}


static inline uint_t get_cr3(void)
{
	uint_t cr3;
	
	__asm__ volatile
	(" \
		movl %%cr3, %%eax; \
		movl %%eax, %0"
	:"=g" (cr3)
	:
	:"eax");
		
	return cr3;
}


static inline void set_pc(void *pc)
{
	 __asm__ volatile (" jmpl *%0" ::"m" (pc));
}


#define bus_inb(addr) \
({                    \
	uint_t _addr = addr;\
	uchar_t _b;         \
	 __asm__ volatile   \
	("                  \
		movl %1, %%edx;   \
		inb %%dx, %%al;   \
		movb %%al, %0;"   \
		:"=b" (_b)        \
		:"g" (_addr)      \
		:"edx", "eax");   \
  _b; })


static inline void bus_outb(uint_t addr, uchar_t b)
{
	 __asm__ volatile
	(" \
		movl %0, %%edx; \
		movb %1, %%al; \
		outb %%al, %%dx"
		:
		:"g" (addr), "b" (b)
		:"eax", "edx");

	return;
}


/* Macro locks interrupts and mutex */
#define lock_cli(m) { cli(); lock(m); }


/* Macro releases mutex and interrupts */
#define unlock_sti(m) { unlock(m); sti(); }


/*
 * Functions operating on segment descriptors and GDT table
 */


static inline ushort_t insert_gdtdesc(uint_t n, uint_t base, uint_t limit, uint_t type)
{
	__asm__ volatile 
	(" \
		movl %0, %%ebx; \
		pushl %%ebx; \
		movl %%ebx, %%edx; \
		andl $0xff000000, %%edx; \
		shrl $0x10, %%ebx; \
		andl $0xff, %%ebx; \
		orl %%edx, %%ebx; \
		movl %1, %%ecx; \
		movl %%ecx, %%edx; \
		andl $0x00000f0000, %%edx; \
		orl %%edx, %%ebx; \
		movl %2, %%ebp; \
		orl %%ebp, %%ebx; \
		andl $0x0000ffff, %%ecx; \
		popl %%edx; \
		shll $0x10, %%edx; \
		orl	%%ecx, %%edx; \
		movl %3, %%eax; \
		movl %%edx, (%%eax); \
		movl %%ebx, 4(%%eax)"
	:
	: "iq" (base), "iq" (limit), "iq" (type), "iq" (KERNEL_BASE + GDT_ADDR + n * 8)
	: "eax", "ebx", "ecx", "edx", "ebp");
	
	return (n * 8);
}


/*
 * CPU context switching functions.
 * CPU context of the task is stored on its kernel stack.
 * Context switching uses kernel stack switching. When current task enters
 * to kernel, it stores all CPU registers on kernel stack, saves its stack
 * pointer in the architecture dependent part of task structure, reloads page
 * directory and switches stack to kernel stack of new scheduled task.
 */


/*
 * This routine is used if on the kernel stack of newwly scheduled task
 * exist only partial startup context
 */
static inline void switch_to(task_t *old, task_t *new)
{
	void *p0, *p1;

	if (old != NULL) {
		p0 = &old->ac;
		__asm__ volatile
		(" \
			movl %0, %%ebx; \
			movl %%esp, (%%ebx)"
		:
		: "m" (p0)
		: "ebx");
	}

	p1 = &new->ac;
	
	__asm__ volatile
	(" \
		movl %0, %%ebx; \
		movl 4(%%ebx), %%eax; \
		movl %%eax, %%cr3; \
		movl (%%ebx), %%esp; \
		movl %%esp, %%eax; \
		addl $0x1000, %%eax; \
		andl $0xfffff000, %%eax; \
		movl %1, %%ebx; \
		movl %%eax, 4(%%ebx); \
		popl %%edi; \
		popl %%esi; \
		popl %%ebp; \
		popl %%edx; \
		popl %%ecx; \
		popl %%ebx; \
		popl %%eax; \
 		popw %%gs; \
		popw %%fs; \
		popw %%es; \
		popw %%ds; \
		iret"
	:
	: "m" (p1), "g" (&common_tss)
	: "eax", "ebx");
	return;	
}


/* Function performs full context switching */
static inline void switch_context(task_t *old, task_t *new)
{
	void *p0, *p1;
	
	if (old != NULL) {
		p0 = &old->ac;	
		__asm__ volatile
		(" \
			movl %0, %%ebx; \
			movl %%esp, (%%ebx)"
		:
		: "m" (p0)
		: "ebx");
	}

	p1 = &new->ac;
	__asm__ volatile
	(" \
		movl %0, %%ebx; \
		movl 4(%%ebx), %%eax; \
		movl %%eax, %%cr3; \
		movl (%%ebx), %%esp; \
		movl %%esp, %%eax; \
		addl $0x1000, %%eax; \
		andl $0xfffff000, %%eax; \
		movl %1, %%ebx; \
		movl %%eax, 4(%%ebx)"
	:
	: "m" (p1), "g" (&common_tss)
	: "eax", "ebx");
	return;	
}


extern void (*_irq0)(uint_t intr);


static inline void reschedule(void)
{
		__asm__ volatile
		(" \
			pushfl; \
			cli; \
			xorl %%eax, %%eax; \
			movw %%cs, %%ax; \
			pushl %%eax; \
			call %0"
		:
		: "m" (_irq0));
	
	return;	
}


/* Function setups CPU task register */
static inline void settr(uint_t sel)
{
	__asm__ volatile
	(" \
		movl %0, %%eax; \
		ltr %%ax"
	:
	: "d" (sel)
	: "eax");
	
	return;	
}


#define __flush_tlb() \
do { unsigned long tmpreg; __asm__ __volatile__("movl %%cr3,%0\n\tmovl %0,%%cr3":"=r" (tmpreg) : :"memory"); } while (0)


#define __hlt()  { __asm__ __volatile__ ("hlt"::); }


/* Macro sends acknowledge to interrupt controler */
#define __intr_end(intr) {                                           \
	if (intr < 8)	bus_outb(0x20, 0x60 | intr);                         \
	else { bus_outb(0x20, 0x62); bus_outb(0xa0, 0x60 | (intr - 8)); }} \


/* Function performs warm reboot */
static inline void reboot(void)
{
	uint_t idtr[4] = { 0, 0, 0, 0 };
	
	__asm__ volatile
	(" \
		lidt %0; \
		xorl %%eax, %%eax; \
		div %%eax"
	: "=m" (idtr)
	: 
	: "eax");
	
	return;	
}


#endif


#endif

