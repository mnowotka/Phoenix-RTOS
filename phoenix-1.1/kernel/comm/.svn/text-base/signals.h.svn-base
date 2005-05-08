/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Signals
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

#ifndef _SIGNALS_H_
#define _SIGNALS_H_

#include <hal/current/types.h>
#include <task/task.h>


#define NSIGNALS    32   /* number of local signals */


/* Signals definition */
#define SIGCHLD     0    /* parent notification about child exit */
#define SIGKILL     1    /* kill signal - immediate task destruction */


/* Local signal handler */ 
extern void *handle_local_signals(uint_t pc);


/* Function installs user level signal handler for current task and given signal */
extern int sigset(uint_t sig, void (*handler)(void));


/* sigset (PSC) */
extern void psc_sigset(uint_t sig, void (*handler)(void), int *err);


#endif
