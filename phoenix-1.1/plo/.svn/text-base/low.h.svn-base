/* 
 * Phoenix-RTOS
 *
 * plo - operating system loader
 *
 * Low-level routines
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

#ifndef _LOW_H_
#define _LOW_H_

#include "types.h"


/* Low-level loader variables */
extern u16 _plo_timeout;

extern void _plo_command(void);

extern int _plo_save(void);


typedef struct {
	u16 addr_lo;
	u16 addr_hi;
	u16 res0;
	u16 res1;
	u16 len_lo;
	u16 len_hi;
	u16 res2;
	u16 res3;
	u16 attr;
	u16 res4;
} low_mmitem_t;


#define MM_MAXSZ   64

typedef struct _syspage_t {
	u8           gdtr[8];
	u8           idtr[8];
	low_mmitem_t mm[MM_MAXSZ];
	u16          mmsize;
} syspage_t;


#define SYSPAGE_OFFS_GDTR        0
#define SYSPAGE_OFFS_IDTR        8
#define SYSPAGE_OFFS_MM          (8 + 8)
#define SYSPAGE_OFFS_MMSIZE      (8 + 8 + 64 * sizeof(low_mmitem_t))


#define IRQ_HANDLED   0
#define IRQ_DEFAULT   1


#define min(a, b)   ((a > b) ? b : a)


extern void low_cli(void);

extern void low_sti();

extern u8 low_inb(u16 addr);

extern void low_outb(u16 addr, u8 b);


extern void low_setfar(u16 segm, u16 offs, u16 v);

extern u16 low_getfar(u16 segm, u16 offs);

extern void low_copyto(u16 segm, u16 offs, void *src, unsigned int l);

extern void low_copyfrom(u16 segm, u16 offs, void *dst, unsigned int l);

extern void low_memcpy(char *dst, char *src, unsigned int l);

extern u16 low_getcs(void);


extern void low_setmode(u8 m);

extern void low_putc(char attr, char c);

extern void low_getc(char *c, char *sc);

extern int low_keypressed(void);


/* Function prepares memory map for kernel */
extern int low_mmcreate(void);

/* Function returns selected memory map item */
extern int low_mmget(unsigned int n, low_mmitem_t *mmitem);

/* Function starts kernel loaded into memory */
extern int low_launch(void);


extern void low_maskirq(u16 n, u8 v);

extern int low_irqinst(u16 irq, int (*isr)(u16, void *), void *data);

extern int low_irquninst(u16 irq);


extern void low_init(void);

extern void low_done(void);


#endif
