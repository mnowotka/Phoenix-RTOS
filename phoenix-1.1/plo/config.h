/* 
 * Phoenix-RTOS
 *
 * plo - operating system loader
 *
 * Configuration
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

#ifndef _CONFIG_H_
#define _CONFIG_H_


#define VERSION "1.1"


/* User interface */
#define WELCOME "-\\- Phoenix-RTOS loader, version: " VERSION ", (c) Pawel Pisarczyk, 2001, 2005"
#define PROMPT  "(plo)% "

#define LINESZ  70
#define HISTSZ  8


/* Console character attributes */
#define ATTR_DEBUG    2
#define ATTR_USER     7
#define ATTR_INIT     13
#define ATTR_LOADER   15
#define ATTR_ERROR    4


/* Kernel file */
#define KERNEL_PATH   "phoenix"


/* Kernel adresses and sizes */
#define KERNEL_BASE  0xc0000000

#define GDT_SEG      0x0100
#define GDT_SIZE     0x0800

#define IDT_SEG      0x0180
#define IDT_SIZE     0x0800

#define SYSPAGE_SEG  0x0200
#define SYSPAGE_SIZE 0x1000

#define INIT_ESP     0x7c00


/* Boot command size */
#define CMD_SIZE   64 


#endif
