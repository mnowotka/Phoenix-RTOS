/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Standard library routines
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

#ifndef _STD_H_
#define _STD_H_

#include <hal/current/locore.h>
#include <stdarg.h>


#define min(a, b)   ((a > b) ? b : a)


extern unsigned int std_strlen(char *s);


extern char *std_itoa(int i, char *buff, uint_t size);


extern char *std_itoah(uint_t i, char *buff, uint_t size, uint_t skip);


extern uint_t std_ahtoi(char *s);


extern int std_strncmp(char *s1, char *s2, uint_t count);


extern void std_putc(uint_t syn, char c);


extern void std_puts(uint_t syn, char *s);


extern void std_printf(char *fmt, ...);


#endif
