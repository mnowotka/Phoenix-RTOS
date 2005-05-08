/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Low-level initialization for IA32 architecure (code called after kernel load)
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

#define __ASSEMBLY__

#include <hal/current/linkage.h>
#include <hal/current/defs.h>


.text

ENTRY(_start)
	movw $16, %ax
	movw %ax, %ss
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movl $0x10000, %esp
	
	/* Kill floppy motor */
	movw $0x3f2, %dx
	xorl %eax, %eax
	outb %al, %dx
 
	/* Disable A20 line mask */
	call empty8042
	movb $0xd1, %al
	outb %al, $0x64
	call empty8042
	movb $0xdf, %al
	outb %al, $0x60
	call empty8042

	/* Test memory size test (output in esi register) */
	movl $0x100000 - 0x1000, %esi

memtest_loop:
	addl $0x1000, %esi
	movl  $0xaa, %ebx
	xchg  (%esi), %ebx
	xchg  (%esi), %ebx
	cmp   $0xaa, %ebx
	jne   memtest_end
	jmp   memtest_loop
memtest_end:

	/* Create kernel page directory and page table */
setup_paging:
	movl $1024 * 2, %ecx
	xorl %eax, %eax
  movl $KERNEL_PAGE_DIR, %edi
  cld;
	rep; stosl

	/* Map first 4MB of memory at KERNEL_BASE and at 0 adresses */
	movl $KERNEL_PAGE_TABLE + 3, KERNEL_PAGE_DIR + 768 * 4
	movl $KERNEL_PAGE_TABLE + 3, KERNEL_PAGE_DIR

	/*
 	 * Create first page table for kernel. Other page tables will be
	 * created later in pmap_init function
 	 */
	movl $KERNEL_PAGE_TABLE + 0x1000 - 4, %edi
	movl $0x03ff003, %eax
	std
1:
	stosl
	subl $0x1000, %eax
	jge 1b
	cld

	/* Now enable paging */
	movl $KERNEL_PAGE_DIR, %eax
	movl %eax, %cr3                
	movl %cr0, %eax
	orl $0x80000000, %eax
	movl %eax, %cr0
	
	/* Store physical memory size in kernel variable */
	movl %esi, physmem_size

	/* Relocate stack, IDT and GDT tables */
	addl $KERNEL_BASE, %esp
	lidt idt_reg
	lgdt gdt_reg

	/* Now jump to the main function... */	
	call trampoline

	/* If all things going well this part of code isn't executed */
	cli
	hlt


/*
 * Function flushes keyboard controler buffer. Code derived from the Linux kernel.
 * Greetings for Linus. :)
 */
ENTRY(empty8042)
	pushf
	pushl %eax
	pushl %ecx
2:
	/* Short delay */
	mov $0x1000, %ecx               
3:
	rep; loop 3b

	/* Test if output buffer is empty */
	inb $0x64, %al
	test $1, %al	
	jz 5f

	/* Second delay */
	mov $0x1000, %ecx
4:
	rep; loop 4b

	/* Read a character */
	in $0x60, %al
	jmp 2b
5:
	/* Test input keyboard buffer - if it isn't empy repeat all steps */
	test $2, %al
	jnz 2b
	pop %eax
	pop %ecx
	popf
	ret
