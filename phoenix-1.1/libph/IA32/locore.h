/*
 * Phoenix-RTOS
 *
 * Standard library
 *
 * Kernel calls
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

#ifndef _LOCORE_H
#define _LOCORE_H

#ifndef __ASSEMBLY__

#include <libph.h>


static inline void __memcpy(void *to, void *from, uint_t n)
{
   __asm__ volatile
	(" \
		cld; \
		movl %0, %%ecx; \
		movl %%ecx, %%edx; \
		andl $3, %%edx; \
		shrl $2, %%ecx; \
		movl %1, %%edi; \
		movl %2, %%esi; \
		rep; movsl; \
		movl %%edx, %%ecx; \
		rep; movsb"
	:
	: "g" (n), "m" (to), "m" (from)
	: "ecx", "edx", "esi", "edi");
}


static inline void __memclr(void *where, uint_t n)
{
   __asm__ volatile
	("cld; \
		xorl	%%eax, %%eax; \
		movl	%0, %%edi; \
		movl  %1, %%ecx; \
		rep; stosb"
	:
	: "m" (where), "g" (n)
	: "eax", "ecx", "edi");
}


static inline void __lock(mutex_t *mutex)
{
   __asm__ volatile
	(" \
	1: \
		xorl %%eax, %%eax; \
		xchgl %0, %%eax; \
		cmp $0, %%eax; \
		jz 1b"
	:
	: "m" (*mutex)
	: "eax");
}


static inline void __unlock(mutex_t *mutex)
{
   __asm__ volatile
	(" \
		xorl %%eax, %%eax; \
		incl %%eax; \
		xchgl %0, %%eax"
	:
	: "m" (*mutex)
	: "eax");
}


static inline void __dev_write(dev_t *dev, char *buff, uint_t *pos, uint_t length)
{
	__asm__
	(" \
		movl $0x01, %%edx; \
		movl %0, %%eax; \
		movl %1, %%ebx; \
		movl %2, %%ecx; \
		movl %3, %%edi; \
		int $0x80"
	:
	:"m" (dev), "m" (buff), "m" (pos), "g" (length) 
	:"eax", "ebx", "ecx", "edx", "edi");
	
	return;
}


static inline void __dev_read(dev_t *dev, char *buff, uint_t *pos, uint_t length)
{
	__asm__
	(" \
		movl $0x02, %%edx; \
		movl %0, %%eax; \
		movl %1, %%ebx; \
		movl %2, %%ecx; \
		movl %3, %%edi; \
		int $0x80"
	:
	:"m" (dev), "m" (buff), "m" (pos), "g" (length) 
	:"edx", "eax", "ebx", "ecx", "edi");
	
	return;
}


static inline void __gettasks(uint_t *pids, uint_t length, uint_t *ntasks)
{
	__asm__ volatile
	(" \
		movl $0x06, %%edx; \
		movl %0, %%eax; \
		movl %1, %%ebx; \
		movl %2, %%ecx; \
		int $0x80"
	:
	:"m" (pids), "g" (length), "m" (ntasks) 
	:"eax", "ebx", "ecx", "edx");
	
	return;
}


static inline int __gettaskinfo(uint_t pid, taskinfo_t *ti)
{
	int err;
	
	__asm__
	(" \
		movl $0x07, %%edx; \
		movl %0, %%eax; \
		movl %1, %%ebx; \
		movl %2, %%ecx; \
		int $0x80"
	:
	:"g" (pid), "g" (ti), "g" (&err) 
	:"eax", "ebx", "ecx", "edx" );
	
	return err;
}


static inline void __getmeminfo(meminfo_t *mi)
{
	__asm__ 
	(" \
		movl $0x08, %%edx; \
		movl %0, %%eax; \
		int $0x80"
	:
	:"m" (mi)
	:"eax", "edx" );
	
	return;
}


static inline int __raise(uint_t pid, uint_t sig)
{
	int err;
	
	__asm__ (" \
		movl $0x09, %%edx; \
		movl %0, %%eax; \
		movl %1, %%ebx; \
		movl %2, %%ecx; \
		int $0x80"
	:
	:"g" (pid), "g" (sig), "g" (&err) 
	:"eax", "ebx", "ecx", "edx" );
	
	return err;
}


static inline int __exec(char *name)
{
	int err;
	
	__asm__
	(" \
		movl $0x0a, %%edx; \
		movl %0, %%eax; \
		movl %1, %%ebx; \
		int $0x80"
	:
	:"m" (name), "g" (&err) 
	:"eax", "ebx", "edx" );
	
	return err;
}


static inline uint_t __wait(int *err)
{
	uint_t pid;
	
	__asm__
	(" \
		movl $0x0b, %%edx; \
		movl %0, %%eax; \
		movl %1, %%ebx; \
		int $0x80"
	:
	:"g" (&pid), "m" (err) 
	:"eax", "ebx", "edx" );
	
	return pid;
}


static inline void __exit(int err)
{
	__asm__
	(" \
		movl $0x0c, %%edx; \
		movl %0, %%eax; \
		int $0x80"
	:
	:"g" (err) 
	:"eax", "edx" );
	
	return;
}


static inline int __sigset(uint_t sig, void (*handler)(void))
{
	int err;
	
	__asm__ 
	(" \
		movl $0x0d, %%edx; \
		movl %0, %%eax; \
		movl %1, %%ebx; \
		movl %2, %%ecx; \
		int $0x80"
	:
	:"g" (sig), "m" (*handler), "g" (&err) 
	:"eax", "ebx", "ecx", "edx" );
	
	return err;
}


static inline void __sleep_unintr(uint_t delay)
{
	__asm__
	(" \
		movl $0x0e, %%edx; \
		movl %0, %%eax; \
		int $0x80"
	:
	:"g" (delay) 
	:"eax", "edx" );
	
	return;
}


static inline void __hal_inject(void *addr, u8 mask, u8 op)
{
	__asm__
	(" \
		movl $0x10, %%edx; \
		movl %0, %%eax; \
		movl %1, %%ebx; \
		movl %2, %%ecx; \
		int $0x80"
	:
	:"g" (addr), "g" (mask), "g" (op)
	:"eax", "ebx", "ecx", "edx" );
	
	return;
}


#endif


#endif
