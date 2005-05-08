/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Architecture dependent definitions
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

#ifndef _DEFS_H_
#define _DEFS_H_


/* Paging parameters */
#define PAGE_SIZE               0x1000      /* page size - 4KB */
#define PAGE_DIR_SIZE           0x0400      /* number of entries in page directory */
#define PAGE_TABLE_SIZE         0x0400      /* number of entries in page table */


/*
 * Physical memory layout
 *
 * 0 KB   ------------------
 *              NULL
 * 4 KB   ------------------
 *              IDT
 * 8 KB   ------------------
 *              GDT
 * 12 KB  ------------------
 *             BIOS_ENV
 * 16 KB  ------------------
 *           KERNEL_PDIR
 * 20 KB  ------------------
 *          KERNEL_PTABLE0
 * 24 KB  ------------------
 *          KERNEL_PTABLE1
 * 28 KB  ------------------
 *          KERNEL_PTABLE2
 * 32 KB  ------------------
 *           KERNEL_STACK 
 * 36 KB  ------------------
 *              kernel
 * 640 KB ------------------
 *               ROM
 *  1MB   ------------------
 *            mem_map
 * sizeof(mem_map)
 *             FREE
 */


#define KERNEL_BASE             0xc0000000                /* base virtual address of kernel space */
#define KERNEL_PAGE_DIR         0x40000                   /* kernel page dir address */
#define KERNEL_PAGE_TABLE       KERNEL_PAGE_DIR + 0x1000  /* first kernel page table address*/
#define KERNEL_CODE_START       0x00000000                /* start of kernel reserved memory */
#define KERNEL_BSS_END          0x100000                  /* begin of kernel heap */
#define USER_STACK_TOP          KERNEL_BASE               /* top of user level stack */


#define SYSINFO_PAGE            (KERNEL_BASE + 0x7e000)


/* Macros used to transform physical and kernel adresses */
#define PHYS_TO_KERNEL(a) ((void *)a + KERNEL_BASE)
#define KERNEL_TO_PHYS(a) ((void *)a - KERNEL_BASE)


/* Macro calculates page dir index from address given by a */
#define GET_PDIR_IDX(a)   ((pdentry_t)a / PAGE_DIR_SIZE / PAGE_SIZE)


#define NULL 0


/* Colors of console messages */
#define KERNMSG_COLOR  0x0f
#define USRMSG_COLOR   7


/* Maximum decimator - used in functions which convert numerical values to strings */
#define MAX_DECIMVAL 1000000000


/* Used selectors */
#define KERNEL_CS   8
#define KERNEL_DS   16
#define USER_CS     27
#define USER_DS     35


/* Adresses of IDT and GDT tables created by loader */
#define IDT_ADDR	0x1800
#define GDT_ADDR	0x1000


/* Bitfields used to construct interrupt descriptors */
#define IGATE_DPL0		0x00000000
#define IGATE_DPL3    0x00006000
#define IGATE_PRES		0x00008000
#define IGATE_SYSTEM	0x00000000
#define	IGATE_IRQEXC	0x00000e00
#define IGATE_TRAP		0x00000f00
#define IGATE_TSS			0x00000500


/* Bitfields used to construct segment descriptors */
#define DG_4KB     0x00800000    /* 4KB segment granularity */
#define DG_1B      0x00000000    /* 1B segment granularity */
#define DC_32      0x00400000    /* 32-bit code segment */
#define DC_16      0x00000000    /* 16-bit code segment */
#define DP_YES     0x00008000    /* present segment */
#define DP_NO      0x00000000    /* segment not present in the physcial memory*/

#define DPL_0      0x00000000    /* kernel privilege level segment */
#define DPL_3      0x00006000    /* user privilege level segment */

#define DU_SYSTEM  0x00000000    /* segment used by system */
#define DU_APP     0x00001000    /* segment used by application */

#define DT_CODE    0x00000800    /* code segment descriptor */
#define DT_DATA    0x00000000    /* data segment descriptor */

#define DDT_EXP    0x00000400    /* data segment is down expandable */
#define DDT_WRT    0x00000200    /* writing is permitted in data segment */
#define DDT_ACC    0x00000100    /* data segment is accesible */ 

#define CDT_CONF   0x00000400    /* conforming code segment */
#define CDT_READ   0x00000200    /* read is permitted in code segment */
#define CDT_ACC    0x00000100    /* code segment is accesible */


/*
 * Predefined descriptor types
 */

/* Descriptor of Task State Segment - used in CPU contex switching */
#define TSS_DESC   (DG_1B | DP_YES | DPL_0 | DU_SYSTEM | 0x00000900)

/* Descriptor of user task code segment */
#define UCODE_DESC (DG_4KB | DC_32 | DP_YES | DPL_3 | DU_APP | DT_CODE | CDT_READ)

/* Descriptor of user task data segment */
#define UDATA_DESC (DG_4KB | DC_32 | DP_YES | DPL_3 | DU_APP | DT_DATA | DDT_WRT)


/* Number of syscalls */
#define NSYSCALLS   17


#endif
