/*
 * Phoenix-RTOS
 *
 * Signale handling test program
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

#include <libph.h>


void sig5(void)
{
	ph_printf("Receive: <signal 5>\n");
}


void sig6(void)
{
	ph_printf("Receive: <signal 6>\n");
}


void sig7(void)
{
	ph_printf("Receive: <signal 7>\n");
}


void sig8(void)
{
	ph_printf("Receive: <signal 8>\n");
}


void sig9(void)
{
	ph_printf("Receive: <signal 9>\n");
}


void sig10(void)
{
	ph_printf("Receive: <signal 10>\n");
}


void sig11(void)
{
	ph_printf("Receive: <signal 11>\n");
}


void sig12(void)
{
	ph_printf("Receive: <signal 12>\n");
}


void _start(void)
{
	ph_sigset(5, sig5);
	ph_sigset(6, sig6);
	ph_sigset(7, sig7);
	ph_sigset(8, sig8);
	ph_sigset(9, sig9);
	ph_sigset(10, sig10);
	ph_sigset(11, sig11);
	ph_sigset(12, sig12);
	
	for (;;)
		ph_sleep(1000);
	
	ph_exit(0);	
}
