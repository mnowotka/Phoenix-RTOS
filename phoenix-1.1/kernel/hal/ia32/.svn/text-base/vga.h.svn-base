/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * VGA register constants, derived from BSD-4.4 kernel
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

#ifndef _VGA_H_
#define _VGA_H_

#define MONO_VRAM     (void *)0xb0000 /* VRAM address of mono 80x25 mode */
#define COLOR_VRAM    (void *)0xb8000 /* VRAM address of color 80x25 mode */

#define MONO_BASE	    0x3b4  /* crtc index register address mono */
#define COLOR_BASE	  0x3d4  /* crtc index register address color */
#define	CRTC_ADDR     0x00   /* index register */

#define CRTC_HTOTAL	   0x00   /* horizontal total */
#define CRTC_HDISPLE	 0x01   /* horizontal display end */
#define CRTC_HBLANKS	 0x02   /* horizontal blank start */
#define CRTC_HBLANKE	 0x03   /* horizontal blank end */
#define CRTC_HSYNCS	   0x04   /* horizontal sync start */
#define CRTC_HSYNCE    0x05   /* horizontal sync end */
#define CRTC_VTOTAL    0x06   /* vertical total */
#define CRTC_OVERFLW   0x07   /* overflow low */
#define CRTC_IROWADDR	 0x08   /* inital row address */
#define CRTC_MAXROW    0x09   /* maximum row address */
#define CRTC_CURSTART	 0x0a   /* cursor start row address */
#define CRTC_CUREND    0x0b   /* cursor end row address */
#define CRTC_STARTADRH 0x0c   /* linear start address mid */
#define CRTC_STARTADRL 0x0d   /* linear start address low */
#define CRTC_CURSORH   0x0e   /* cursor address mid */
#define CRTC_CURSORL   0x0f   /* cursor address low */
#define CRTC_VSYNCS    0x10   /* vertical sync start */
#define CRTC_VSYNCE    0x11   /* vertical sync end */
#define CRTC_VDE       0x12   /* vertical display end */
#define CRTC_OFFSET    0x13   /* row offset */
#define CRTC_ULOC      0x14   /* underline row address */
#define CRTC_VBSTART   0x15   /* vertical blank start */
#define CRTC_VBEND     0x16   /* vertical blank end */
#define CRTC_MODE      0x17   /* CRTC mode register */
#define CRTC_SPLITL    0x18   /* split screen start low */

#define MAIN_MISCIN    0x3cc  /* miscellaneous input register */


#endif
