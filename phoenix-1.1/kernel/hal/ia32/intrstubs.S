/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Low-level stubs for interrupts, exceptions and system calls
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

#define __ASSEMBLY__

#include <hal/current/linkage.h>
#include <hal/current/locore.h>
#include <hal/current/defs.h>


.text

/* Exception handling macro */
#define EXCHAND(exc) \
	pushw %ds               ;\
	pushw %es               ;\
	pushw %fs               ;\
	pushw %gs               ;\
	pushl	%eax              ;\
	pushl %ebx              ;\
	pushl	%ecx							;\
	pushl	%edx							;\
	pushl %ebp              ;\
	pushl %esi              ;\
	pushl %edi              ;\
	movl $KERNEL_DS, %eax   ;\
	movw %ax, %ds           ;\
	movw %ax, %es           ;\
	movw %ax, %fs           ;\
	movw %ax, %gs           ;\
	;\
	/* Call exception handler */ ;\
	leal 0(%esp), %eax; \
	pushl %eax; \
	movl exc_handlers + (exc) * 4, %eax ;\
	pushl $exc ;\
	call *%eax ;\
	addl $8, %esp ;\
	;\
	popl %edi               ;\
	popl %esi               ;\
	popl %ebp               ;\
	popl %edx               ;\
	popl %ecx							  ;\
	popl %ebx               ;\
	popl %eax               ;\
	popw %gs                ;\
	popw %fs                ;\
	popw %es                ;\
	popw %ds                ;\
	iret                    ;


/* Exception stubs */
ENTRY(_exc0);
	pushl $0;
	EXCHAND(0);

ENTRY(_exc1);
	pushl $0;
	EXCHAND(1);

ENTRY(_exc2);
	pushl $0;
	EXCHAND(2);

ENTRY(_exc3);
	pushl $0;
	EXCHAND(3);

ENTRY(_exc4);
	pushl $0;
	EXCHAND(4);

ENTRY(_exc5);
	pushl $0
	EXCHAND(5);

ENTRY(_exc6);
	pushl $0
	EXCHAND(6);

ENTRY(_exc7);
	pushl $0
	EXCHAND(7);

ENTRY(_exc8);
	EXCHAND(8);

ENTRY(_exc9);
	pushl $0
	EXCHAND(9);

ENTRY(_exc10);
	EXCHAND(10);

ENTRY(_exc11);
	EXCHAND(11);

ENTRY(_exc12);
	EXCHAND(12);

ENTRY(_exc13);
	EXCHAND(13);

ENTRY(_exc14);
	EXCHAND(14);

ENTRY(_exc15);
	pushl $0
	EXCHAND(15);

ENTRY(_exc16);
	pushl $0
	EXCHAND(16);

ENTRY(_exc17);
	EXCHAND(17);

ENTRY(_exc18);
	pushl $0
	EXCHAND(18);


/* Interrupt handling macro */
#define	INTRHAND(intr) \
	pushw %ds               ;\
	pushw %es               ;\
	pushw %fs               ;\
	pushw %gs               ;\
	pushl	%eax              ;\
	pushl %ebx              ;\
	pushl	%ecx							;\
	pushl	%edx							;\
	pushl %ebp              ;\
	pushl %esi              ;\
	pushl %edi              ;\
	movl $KERNEL_DS, %eax   ;\
	movw %ax, %ds           ;\
	movw %ax, %es           ;\
	movw %ax, %fs           ;\
	movw %ax, %gs           ;\
                          ;\
	/* Call interrupt handler */               ;\
	movl intr_handlers + (intr) * 4, %eax			 ;\
	movl $intr, %ebx        ;\
	pushl %ebx						  ;\
	call *%eax 			        ;\
	addl $4,%esp						;\
                          ;\
	/* Obtain eip value and handle signals */   ;\
	movl 36(%esp), %eax                        ;\
	pushl %eax                                 ;\
	call handle_local_signals                  ;\
	addl $4, %esp                              ;\
	cmpl $0, %eax                              ;\
	je 1f                                      ;\
	                                           ;\
	/* If handling function is defined prepare user stack */ ;\
	movl 48(%esp), %ebx     ;\
	movl 36(%esp), %ecx     ;\
	movl %eax, 36(%esp)     ;\
	subl $4, %ebx           ;\
	movl %ecx, (%ebx)       ;\
	movl %ebx, 48(%esp)     ;\
1:                        ;\
	popl %edi               ;\
	popl %esi               ;\
	popl %ebp               ;\
	popl %edx               ;\
	popl %ecx							  ;\
	popl %ebx               ;\
	popl %eax               ;\
	popw %gs                ;\
	popw %fs                ;\
	popw %es                ;\
	popw %ds                ;\
	iret                    ;


/* Scheduler stub code */
ENTRY(_irq0)
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	pushl	%eax
	pushl %ebx
	pushl	%ecx
	pushl	%edx
	pushl %ebp
	pushl %esi
	pushl %edi
	movl $KERNEL_DS, %eax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	movl intr_handlers, %eax
	pushl $0
	call *%eax
	addl $4, %esp

	/* Obtain eip value and handle signals */
	movl 36(%esp), %eax
	pushl %eax
	call handle_local_signals
	addl $4, %esp
	cmpl $0, %eax
	je 1f
	
	/* If handling function is defined prepare user stack */
	movl 48(%esp), %ebx
	movl 36(%esp), %ecx
	movl %eax, 36(%esp)
	subl $4, %ebx
	movl %ecx, (%ebx)
	movl %ebx, 48(%esp)

1:
	popl %edi
	popl %esi
	popl %ebp
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	popw %gs
	popw %fs
	popw %es
	popw %ds
	iret


/* Other interrupts stubs */
ENTRY(_irq1);
	INTRHAND(1);
ENTRY(_irq2)
	INTRHAND(2);
ENTRY(_irq3)
	INTRHAND(3);
ENTRY(_irq4);
	INTRHAND(4);
ENTRY(_irq5)
	INTRHAND(5);
ENTRY(_irq6)
	INTRHAND(6);
ENTRY(_irq7);
	INTRHAND(7);
ENTRY(_irq8)
	INTRHAND(8);
ENTRY(_irq9)
	INTRHAND(9);
ENTRY(_irq10);
	INTRHAND(10);
ENTRY(_irq11)
	INTRHAND(11);
ENTRY(_irq12)
	INTRHAND(12);
ENTRY(_irq13);
	INTRHAND(13);
ENTRY(_irq14)
	INTRHAND(14);
ENTRY(_irq15)
	INTRHAND(15);


ENTRY (_syscall)
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	pushl %eax
	pushl	%eax
	movl $KERNEL_DS, %eax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	popl %eax
	cmpl $NSYSCALLS, %edx
	jge err_exit
	pushl %esi
	pushl %edi
	pushl %ecx
	pushl %ebx
	pushl %eax
	movl %edx, %eax
	shll $4, %eax
	addl $sysents, %eax
	movl (%eax), %eax
	call *%eax
	addl $20, %esp

	/* Obtain eip value and handle signals */
	movl 12(%esp), %eax
	pushl %eax
	call handle_local_signals
	addl $4, %esp
	cmpl $0, %eax
	je err_exit
	
	/* If handling function is defined prepare user stack */
	pushl %ebx
	pushl %ecx
	movl 32(%esp), %ebx
	movl 20(%esp), %ecx
	movl %eax, 20(%esp)
	subl $4, %ebx
	movl %ecx, (%ebx)
	movl %ebx, 32(%esp)
	popl %ecx
	popl %ebx

err_exit:
	popl %eax
	popw %gs
	popw %fs
	popw %es
	popw %ds
	iret
