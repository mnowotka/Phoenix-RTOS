/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Exception and interrupt handling
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
#include <task/if.h>
#include <comm/signals.h>


extern void (*_exc0)(uint_t exc);
extern void (*_exc1)(uint_t exc);
extern void (*_exc2)(uint_t exc);
extern void (*_exc3)(uint_t exc);
extern void (*_exc4)(uint_t exc);
extern void (*_exc5)(uint_t exc);
extern void (*_exc6)(uint_t exc);
extern void (*_exc7)(uint_t exc);
extern void (*_exc8)(uint_t exc);
extern void (*_exc9)(uint_t exc);
extern void (*_exc10)(uint_t exc);
extern void (*_exc11)(uint_t exc);
extern void (*_exc12)(uint_t exc);
extern void (*_exc13)(uint_t exc);
extern void (*_exc14)(uint_t exc);
extern void (*_exc15)(uint_t exc);
extern void (*_syscall)(void);
extern void (*_irq0)(uint_t intr);
extern void (*_irq1)(uint_t intr);
extern void (*_irq2)(uint_t intr);
extern void (*_irq3)(uint_t intr);
extern void (*_irq4)(uint_t intr);
extern void (*_irq5)(uint_t intr);
extern void (*_irq6)(uint_t intr);
extern void (*_irq7)(uint_t intr);
extern void (*_irq8)(uint_t intr);
extern void (*_irq9)(uint_t intr);
extern void (*_irq10)(uint_t intr);
extern void (*_irq11)(uint_t intr);
extern void (*_irq12)(uint_t intr);
extern void (*_irq13)(uint_t intr);
extern void (*_irq14)(uint_t intr);
extern void (*_irq15)(uint_t intr);


/* Exception names dictionary */
char *exception_names[] = { "Divide by zero or divide overflow",
                            "Debug trap",
                            "NMI exception - fatal error",
                            "Breakpoint",
                            "exception 4",
                            "exception 5",
                            "exception 6",
                            "exception 7",
                            "exception 8",
                            "Invalid TSS",
                            "segment not present",
                            "exception 11",
                            "exception 12",
                            "General protection fault",
                            "Page fault",
                            "exception 15" };


void *exc_handlers[16];
void *intr_handlers[16];


void dump_regs(exc_context_t *ctx)
{
	std_printf("\n");
	std_printf("eax=%p ebx=%p ecx=%p edx=%p ebp=%p\n", ctx->eax, ctx->ebx, ctx->ecx, ctx->edx, ctx->ebp);
	std_printf("esi=%p edi=%p esp=%p eip=%p\n\n", ctx->esi, ctx->edi, ctx->esp, ctx->eip);

	std_printf("cs=%p  es=%p  gs=%p\n", ctx->cs, ctx->es, ctx->gs);
	std_printf("ds=%p  fs=%p  ss=%p\n\n", ctx->ds, ctx->fs, ctx->ss);
	
	std_printf("eflags=%p  cr0=%p  cr2=%p  cr3=%p\n\n",
						(void *)ctx->eflags, (void *)get_cr0(), (void *)get_cr2(), (void *)get_cr3());

	std_printf("Code:\n");
	hal_disasm(ctx->eip);
	
	return;
};


void dummy_exc_handler(u32 exc, exc_context_t *ctx)
{
	uint_t stack_top;
	task_t *task;
	
	task = __scheduler_getcurrent();
	
	if (!task) {
		std_printf("KERNEL PANIC [%d] [%s] at 0x%p\n", exc, exception_names[exc], (void *)*(uint_t *)((void *)&stack_top + 16));
		dump_regs(ctx);
		__hlt();
	}

	std_printf("\nException %p at %p\n", exc, ctx->eip);
	
	if (exc == 3) {
		hal_preinject((void *)ctx->eip);
		ctx->eip = ctx->eip - 1;
		
		/* Set step mode */
		ctx->eflags |= 0x00000100;
 
		return;
	}
	
	if (exc == 1) {
		hal_postinject((void *)ctx->eip);
		
		/* Clear step mode */
		ctx->eflags &= ~0x00000100;
		return;
	}

	dump_regs(ctx);
for (;;);

	if (exc == 14)
		std_printf("Unallowed acces to 0x%p\n", (void *)get_cr2());
			
	exit_task(-1);
	
	return;
}


void dummy_intr_handler(uint_t intr)
{
	std_printf("KERNEL DEBUG interrupt [%d]\n", intr);	
	return;
}


/* Function setups interrupt stub in IDT */
void set_stub(uint_t i, void *addr)
{
	uint_t offs1;
	uint_t offs2;

	offs1 = ((int)addr & 0xffff0000);
	offs2 = ((int)addr & 0x0000ffff);
	
	offs1 |= IGATE_DPL0 | IGATE_PRES | IGATE_SYSTEM | IGATE_IRQEXC;
	offs2 |= (KERNEL_CS << 16);
	
	*(int *)((char *)(KERNEL_BASE + IDT_ADDR) + i * 8 + 4) = offs1;
	*(int *)((char *)(KERNEL_BASE + IDT_ADDR) + i * 8) = offs2;
	
	return;
}


/* Function setups trap stub in IDT */
void set_trap_stub(uint_t i, void *addr)
{
	uint_t offs1;
	uint_t offs2;

	offs1 = ((int)addr & 0xffff0000);
	offs2 = ((int)addr & 0x0000ffff);
	
	offs1 |= IGATE_DPL3 | IGATE_PRES | IGATE_SYSTEM | IGATE_TRAP;
	offs2 |= (KERNEL_CS << 16);
	
	*(int *)((char *)(KERNEL_BASE + IDT_ADDR) + i * 8 + 4) = offs1;
	*(int *)((char *)(KERNEL_BASE + IDT_ADDR) + i * 8) = offs2;
	
	return;
}


void set_exc_handler(uint_t exc, void *handler)
{
	exc_handlers[exc] = handler;
	return;
}


void set_intr_handler(uint_t intr, void *handler)
{
	intr_handlers[intr] = handler;
	return;
}


/* Function initializes excetpion and interrupt handling */
void interrupts_init(void)
{
	uint_t k;
	
	/* Initialize interrupt controllers */
	bus_outb(0x20, 0x11);  /* ICW1 */
	bus_outb(0x21, 0x20);  /* ICW2 (Master) */
	bus_outb(0x21, 0x04);  /* ICW3 (Master) */
	bus_outb(0x21, 0x15);  /* ICW4 */
	bus_outb(0xa0, 0x11);  /* ICW1 (Slave) */
	bus_outb(0xa1, 0x28);  /* ICW2 (Slave) */
	bus_outb(0xa1, 0x02);  /* ICW3 (Slave) */
	bus_outb(0xa1, 0x19);  /* ICW4 (Slave) */
	
	/* Set exception stubs */
	set_stub(0, &_exc0);
	set_stub(1, &_exc1);
	set_stub(2, &_exc2);
	set_stub(3, &_exc3);
	set_stub(4, &_exc4);
	set_stub(5, &_exc5);
	set_stub(6, &_exc6);
	set_stub(7, &_exc7);
	set_stub(8, &_exc8);
	set_stub(9, &_exc9);
	set_stub(10, &_exc10);
	set_stub(11, &_exc11);
	set_stub(12, &_exc12);
	set_stub(13, &_exc13);
	set_stub(14, &_exc14);
	set_stub(15, &_exc15);
	
	/* Set interrupt stubs */
	set_stub(32, &_irq0);
	set_stub(33, &_irq1);
	set_stub(34, &_irq2);
	set_stub(35, &_irq3);
	set_stub(36, &_irq4);
	set_stub(37, &_irq5);
	set_stub(38, &_irq6);
	set_stub(39, &_irq7);
	set_stub(40, &_irq8);
	set_stub(41, &_irq9);
	set_stub(42, &_irq10);
	set_stub(43, &_irq11);
	set_stub(44, &_irq12);
	set_stub(45, &_irq13);
	set_stub(46, &_irq14);
	set_stub(47, &_irq15);
	
	/* Syscall trap */
	set_trap_stub(128, &_syscall);
	
	/* Set default exception and interrupt handlers */
	for (k = 0; k < 15; k++)
		set_exc_handler(k, (void *)&dummy_exc_handler);

	for (k = 0; k < 15; k++)
		set_intr_handler(k, (void *)&dummy_intr_handler);
	return;
}
