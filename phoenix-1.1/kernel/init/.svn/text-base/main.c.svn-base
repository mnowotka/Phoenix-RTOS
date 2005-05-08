/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * High-level kernel initialization
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
#include <task/timesys.h>
#include <task/scheduler.h>
#include <task/exec.h>
#include <dev/drivers.h>
#include <dev/serial.h>
#include <dev/tty.h>


/* Physical memory size - variable is set by init sequence */
extern uint_t physmem_size;


int task_idle(void)
{
	for (;;) {
		__hlt();
	}		 
	return 0;
}


int task_run(void)
{
	uint_t k;

	/* Run idle tasks */
	for (k = 0; k < 2; k++)
		create_kernel_thread("idle", task_idle, NULL, 0);

	/* Execute Phoenix shell */
	exec("psh");
	
	/* Suspend task execution. Kernel thread can't be stopped. */
	for (;;)
		sleep_unintr(1000);

	return 0;
}


int main(void)
{
	task_t *run_task;
	int color;
	
	/* Initialize interrupts and exception */
	interrupts_init();
	
	/* Initialize system console */
	color = console_init(TEXT80x25);
	console_clear();
	
	/* Print welcome information */
	std_printf("-\\- Phoenix microkernel, ver. 1.1, (c) Pawel Pisarczyk, 2001, 2005\n");
	std_printf("-----------------------------------------------------------------\n");
	std_printf("disabling A20 gate\n");
	std_printf("testing physical memory [%d KB detected]\n", physmem_size / 1024);
	std_printf("starting paging\n");
	std_printf("enabling interrupts\n");
	
	std_printf("testing monitor type ");
	if (color)
		std_printf("[VGA-color detected]\n");
	else
		std_printf("[VGA-mono detected]\n");

	/* Initialize virtual memory managment */
	std_printf("initializing pmap interface\n");	
	pmap_init(physmem_size);		
	init_mem_map(physmem_size);		
	std_printf("creating memory map\n");
	std_printf("initializing kmalloc subsystem\n");	
	kmalloc_init(2);	
	disp_meminfo();
	
	/* Initialize system timer */
	if (timesys_init(10000) < 0) {
		std_printf("KERNEL PANIC! Can't init timesys. Probably bad timeslice value!\n");
		for (;;)
			__hlt()
	}

	/* Initialize scheduler */
	scheduler_init();
		
	/* Initialize drivers */
	drivers_init();	
	serial_init(BPS_115200);
	tty_init();
	
	/* Unmask interrupts */
	sti();
	run_task = create_kernel_thread("runthr", task_run, NULL, 0);

	/* Wait for first context switch */
	for (;;);	

	return 0;
}


void trampoline(void)
{
	set_pc(&main);
}
