/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Debug console
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

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <hal/current/types.h>


#define CON_NSYN   0
#define CON_SYN    1

#define CONSOLE_INTR  1


/* Console screen modes */
#define TEXT80x25       1


/*
 * Function initializes console in specified mode. At this time
 * only one TEXT80x25 mode is allowed. Functions returns detected monitor
 * type - color or mono.
 */
extern int console_init(uint_t mode);


/* Function clears console */
extern int console_clear(void);


/* Function locks access to console for other tasks - prevents debug messages mixing */
extern void console_lock(void);


/* Function releases console access */
extern void console_unlock(void);


/* Function draws string on console */
extern void console_puts(uint_t syn, char *s);


/* Functions gets characters from the keyboard */
extern char *keyb_get(void);


#endif
