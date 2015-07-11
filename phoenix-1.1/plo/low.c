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

#include "config.h"
#include "errors.h"
#include "low.h"
#include "plostd.h"


extern void _irq0(void);
extern void _irq1(void);
extern void _irq2(void);
extern void _irq3(void);
extern void _irq4(void);
extern void _irq5(void);
extern void _irq6(void);
extern void _irq7(void);
extern void _irq8(void);
extern void _irq9(void);
extern void _irq10(void);
extern void _irq11(void);
extern void _irq12(void);
extern void _irq13(void);
extern void _irq14(void);
extern void _irq15(void);


void *irqstubs[] = {
	_irq0, _irq1, _irq2, _irq3, _irq4, _irq5, _irq6, _irq7,
	_irq8, _irq9, _irq10, _irq11, _irq12, _irq13, _irq14, _irq15
};


struct {
	void *irqseg[16];
	void *irqoffs[16];
	void *data[16];
	void *isr[16];
} irqdata;


void low_cli(void)
{
#asm
	cli
#endasm
}


void low_sti()
{
#asm
	sti
#endasm
}


u8 low_inb(u16 addr)
{
#asm
	push bp
	mov bp, sp
	push dx
	
	mov dx, 4[bp]
	in al, dx
	
	pop dx
	pop bp
#endasm
}


void low_outb(u16 addr, u8 b)
{
#asm
	push bp
	mov bp, sp
	push dx
	
	mov dx, 4[bp]
	mov al, 6[bp]
	out dx, al
	
	pop dx
	pop bp
#endasm
}


void low_setfar(u16 segm, u16 offs, u16 v)
{
#asm
	push bp
	mov bp, sp
	push bx
	push cx
	push dx
	push es
	
	mov dx, es
	mov ax, 4[bp]
	mov es, ax
	mov bx, 6[bp]
	mov cx, 8[bp]
	seg es
	mov word ptr [bx], cx
	mov es, dx
	
	pop es
	pop dx
	pop cx
	pop bx
	pop bp
#endasm
}


u16 low_getfar(u16 segm, u16 offs)
{
#asm
	push bp
	mov bp, sp
	push bx
	push dx
	
	mov dx, es
	mov ax, 4[bp]
	mov es, ax
	mov bx, 6[bp]
	seg es
	mov ax, word ptr [bx]
	mov es, dx
	
	pop dx
	pop bx
	pop bp
#endasm	
}


void low_copyto(u16 segm, u16 offs, void *src, unsigned int l)
{
#asm
	push bp
	mov bp, sp
	push cx
	push dx
	push si
	push di
	
	mov dx, es
	mov ax, 4[bp]
	mov es, ax
	mov cx, 10[bp]
	mov di, 6[bp]
	mov si, 8[bp]
	rep
	movsb
	mov es, dx
	
	pop di
	pop si
	pop dx
	pop cx
	pop bp
#endasm
}


void low_copyfrom(u16 segm, u16 offs, void *dst, unsigned int l)
{
#asm
	push bp
	mov bp, sp
	push cx
	push dx
	push si
	push di
	
	mov dx, ds
	mov ax, 4[bp]
	mov ds, ax
	mov cx, 10[bp]
	mov si, 6[bp]
	mov di, 8[bp]
	rep
	movsb
	mov ds, dx
	
	pop di
	pop si
	pop dx
	pop cx
	pop bp
#endasm	
}


void low_memcpy(char *dst, char *src, unsigned int l)
{
#asm
	push bp
	mov bp, sp	
	push cx
	push si
	push di

	mov cx, 8[bp]
	mov di, 4[bp]
	mov si, 6[bp]
	rep
	movsb

	pop di
	pop si
	pop cx
	pop bp
#endasm
}


u16 low_getcs(void)
{
#asm
	mov ax, cs
#endasm
}


void low_setmode(u8 m)
{
#asm
	push bp
	mov bp, sp
	
	mov ah, #0
	mov al, 4[bp]
	int 0x10;
	
	pop bp
#endasm
}


void low_putc(char attr, char c)
{
#asm
	push bp
	mov bp, sp
	push bx
	push cx

	cmp 6[bp], #0x0a
	jnz low_putc_l1
	
; Perform carriage return
	xor bx, bx
	mov bl, 4[bp]
	mov al, #0x0d
	mov ah, #0x0e
	mov cx, #1
	int 0x10
	
low_putc_l1:
	xor bx, bx
	mov bl, 4[bp]
	mov al, 6[bp]
	mov ah, #0x0e
	mov cx, #1
	int 0x10

	pop cx
	pop bx
	pop bp
#endasm
}


void low_getc(char *c, char *sc)
{
#asm
	push bp
	mov bp, sp
	push bx
	
	mov ax, #0
	int 0x16
	mov bx, 4[bp]
	mov [bx], al
	mov bx, 6[bp]
	mov [bx], ah
	
	pop bx
	pop bp
#endasm
}


int low_keypressed(void)
{
#asm
	mov ax, #0x0100
	int 0x16
	jnz low_keypressed_l1
	xor ax, ax
	jmp low_keypressed_l2
low_keypressed_l1:
	mov ax, #1;
low_keypressed_l2:
#endasm
}


/* Interrupt handling */


int low_irqdispatch(u16 irq)
{
	int (*isr)(u16, void *);

	if (irqdata.isr[irq] == NULL)
		return IRQ_DEFAULT;
	
	isr = irqdata.isr[irq];
	return isr(irq, irqdata.data[irq]);
}


void low_maskirq(u16 n, u8 v)
{
	u8 m, s;

	m = ~(1 << (n & 7));
	s = low_inb((n < 8) ? 0x21 : 0xa1) & m | ((v & 1) << (n & 7));

	low_outb((n < 8) ? 0x21 : 0xa1, s);
	return;
}


int low_irqinst(u16 irq, int (*isr)(u16, void *), void *data)
{
	if (irq > 15)
		return ERR_ARG;
	
	low_cli();
	irqdata.isr[irq] = (void *)isr;
	irqdata.data[irq] = data;
	low_sti();
	return 0;
}


int low_irquninst(u16 irq)
{
	if (irq > 15)
		return ERR_ARG;
	
	low_cli();
	irqdata.isr[irq] = NULL;
	irqdata.data[irq] = NULL;
	low_sti();
	return 0;
}


void low_irqinit(void)
{
	unsigned int k;

	/* Install IRQ stubs */
	low_cli();  
	for (k = 0; k < 16; k++) {
		irqdata.irqoffs[k] = (void *)low_getfar(0, (k + (k < 8 ? 8 : 0x68)) * 4);
		irqdata.irqseg[k] = (void *)low_getfar(0, (k + (k < 8 ? 8 : 0x68)) * 4 + 2);
		
		low_setfar(0, (k + (k < 8 ? 8 : 0x68)) * 4, (u16)irqstubs[k]);
		low_setfar(0,  (k + (k < 8 ? 8 : 0x68)) * 4 + 2, low_getcs());
		
		irqdata.isr[k] = NULL;
		irqdata.data[k] = NULL;
	} 
	low_sti();
	return;
}


void low_irqdone(void)
{
	unsigned int k;

	low_cli();
	
	/* Restore interrupt vector table */
	for (k = 0; k < 8; k++) {
		low_setfar(0, (k + (k < 8 ? 8 : 0x68)) * 4, (u16)irqdata.irqoffs[k]);
		low_setfar(0, (k + (k < 8 ? 8 : 0x68)) * 4 + 2, (u16)irqdata.irqseg[k]);
	}
	
	low_sti();
	return;
}


/* Kernel specific functions */


/* Function starts kernel loaded into memory */
int low_launch(void)
{
	low_cli();

	/* Prepare ring 0 code segment descriptor - selector 0x08 */
	low_setfar(GDT_SEG, 8, 0xffff);
	low_setfar(GDT_SEG, 10, 0x0000);
	low_setfar(GDT_SEG, 12, 0x9a00);
	low_setfar(GDT_SEG, 14, 0x00cf);
		
	/* Prepare ring 0 data segment descriptor - selector 0x10 */
	low_setfar(GDT_SEG, 16, 0xffff);
	low_setfar(GDT_SEG, 18, 0x0000);
	low_setfar(GDT_SEG, 20, 0x9200);
	low_setfar(GDT_SEG, 22, 0x00cf);
	
	/* Prepeare GDTR pseudodescriptor */
	low_setfar(SYSPAGE_SEG, SYSPAGE_OFFS_GDTR + 0, GDT_SIZE);
	low_setfar(SYSPAGE_SEG, SYSPAGE_OFFS_GDTR + 2, GDT_SEG << 4);
	low_setfar(SYSPAGE_SEG, SYSPAGE_OFFS_GDTR + 4, GDT_SEG >> 12);

	/* Prepeare IDTR pseudodescriptor */
	low_setfar(SYSPAGE_SEG, SYSPAGE_OFFS_IDTR + 0, IDT_SIZE);
	low_setfar(SYSPAGE_SEG, SYSPAGE_OFFS_IDTR + 2, IDT_SEG << 4);
	low_setfar(SYSPAGE_SEG, SYSPAGE_OFFS_IDTR + 4, IDT_SEG >> 12);
	
	/* Set text mode */
	low_setmode(0x03);

	/* Load protected mode specific registers */
#asm
	mov ax, #SYSPAGE_SEG
	mov es, ax
	seg es
	lgdt 0

	/* mov eax, cr0 */
	db 0x0f, 0x20, 0xc0

	/* or eax, 1 */
	db 0x66, 0x83, 0xc8, 0x01

	/* mov cr0, eax */
	db 0x0f, 0x22, 0xc0

	/* jmp far 0008h:00060000 */
	db 0x66, 0xea, 0x00, 0x00, 0x06, 0x00, 0x08, 0x00
#endasm

	return;
}


/* Function retrieves memory map item and returns id for next call */ 
int low_int15e820(u32 id, low_mmitem_t *mmitem, u32 *nid)
{
#asm
	push bp
	mov bp, sp
	push ebx
	push ecx
	push edx
	push di
	
	mov eax, #0xe820
	mov ebx, 4[bp]
	mov ecx, #20
	mov edx, #0x534d4150
	mov di, 8[bp]
	int 0x15

	jnc bios15e820_success
	mov ax, #ERR_LOW_BIOS;
	jmp bios15e820_ret

bios15e820_success:
	mov di, 10[bp]
	mov [di], ebx
	xor ax, ax
	
bios15e820_ret:
	pop di
	pop edx
	pop ecx
	pop ebx
	pop bp
#endasm
}


/* Function prepares memory map for kernel */
int low_mmcreate(void)
{
	low_mmitem_t mmitem;
	unsigned int k;
	u32 id;

	low_setfar(SYSPAGE_SEG, SYSPAGE_OFFS_MMSIZE, 0);
	
	for (id = 0, k = 0; k < MM_MAXSZ; k++) {
		if (low_int15e820(id, &mmitem, &id) < 0)
			return ERR_LOW_BIOS;

		low_copyto(SYSPAGE_SEG, SYSPAGE_OFFS_MM + k * sizeof(low_mmitem_t), &mmitem, sizeof(low_mmitem_t));
		low_setfar(SYSPAGE_SEG, SYSPAGE_OFFS_MMSIZE, k + 1);
		if (!id)
			return ERR_OK;
	}

	return ERR_LOW_MMSIZE;
}


/* Function returns selected memory map item */
int low_mmget(unsigned int n, low_mmitem_t *mmitem)
{
	unsigned int mmsize;

	mmsize = low_getfar(SYSPAGE_SEG, SYSPAGE_OFFS_MMSIZE);
	if (n >= mmsize)
		return ERR_LOW_MMSIZE;
	low_copyfrom(SYSPAGE_SEG, SYSPAGE_OFFS_MM + n * sizeof(low_mmitem_t), mmitem, sizeof(low_mmitem_t));

	return ERR_OK;
}


/* Initialization */


void low_init(void)
{
	int res;
	int k;

	/* Set graphics mode */
	low_setmode(0x12);
	
	if ((res = low_mmcreate()) < 0) {
		plostd_printf(ATTR_ERROR, "Problem in memory map creation [res=%x]!\n", res);
		if (res == ERR_LOW_BIOS)
			plostd_printf(ATTR_ERROR, "BIOS doesn't support int 15h, ax=e820h!\n");
		else
			plostd_printf(ATTR_ERROR, "Memory map has to many items >%d!\n", MM_MAXSZ);
		plostd_printf(ATTR_ERROR, "Phoenix can work unstable!\n");
	}

	/* Initialize interrupts */
	low_irqinit();
	return;
}


void low_done(void)
{
	low_irqdone();
	return;
}
