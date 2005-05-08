; 
; Phoenix-RTOS
;
; plo - operating system loader
;
; Loader startup code
;
; Copyright 2001, 2005 Pawel Pisarczyk
;
; This file is part of Phoenix-RTOS.
;
; Phoenix-RTOS is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; Phoenix-RTOS kernel is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with Phoenix-RTOS kernel; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;

.text

.extern _plo_init

.globl _main
_main:
	br __plo_start
.ascii "PLO.210 "

; Loader variables
.globl __plo_timeout
__plo_timeout:
	dw 0005
.globl __plo_command
__plo_command:
.ascii "load com1; go!"
.zerow 64

.globl __plo_start
__plo_start:
	cli	
	mov ax, #0x7c0
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, #0xffff
	sti
	push ax
	mov ax, #__plo_boot
	push ax
	retf

__plo_boot:
	push es

; Read remaining loader sectors (one track)
	xor ax, ax
	xor dx, dx
	int 0x13
	mov ax, #0x7e0
	mov es, ax

__plo_read:
	xor bx, bx
	mov al, #17
	mov ah, #2
	mov cx, #2
	mov dx, #0
	int 0x13
	jc __plo_read

; Read next track
__plo_read1:
	add bx, #17 * 512
	mov al, #18
	mov ah, #2
	mov cx, #1
	mov dx, #0x0100
	int 0x13
	jc __plo_read1

; If loader boots from floppy disk stop motor
	mov dx, #0x3f2
	xor al, al
	out dx, al
	pop es

; When loader exist in memory call its high level part
	call _plo_init

; Reboot after exit
	mov ax, #0xffff
	push ax
	xor ax, ax
	push ax
	retf

; Function saves first sector to disk
.globl __plo_save
__plo_save:
	push bx
	push cx
	push dx

	xor bx, bx
	mov al, #1
	mov ah, #3
	mov cx, #1
	mov dx, #0
	int 0x13
	jc __plo_save_l1
	xor ax, ax

__plo_save_l1:
	pop dx
	pop cx
	pop bx
	ret

.org 0x1fe
	db 0x55, 0xaa

end
