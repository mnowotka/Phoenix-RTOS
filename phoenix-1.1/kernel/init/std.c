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

#include <stdarg.h>
#include <hal/current/if.h>


char *digits = "0123456789";
char *digitsh = "0123456789abcdef";


unsigned int std_strlen(char *s)
{
	unsigned int k;
	
	for (k = 0; *s; s++, k++);
	
	return k;
}


char *std_itoa(int i, char *buff, uint_t size)
{
	int l;
	int div = MAX_DECIMVAL;
	int offs = 0;
	int v;
	int was = 0;

	if (size < 2)
		return NULL;
			
	if (i >= 0) {
		v = i;
		l = 0;
	}
	else {
		v = -i;
		l = 1;
		*(buff) = '-';
	}
	
	while (div) {
		offs = v / div;

		/* First occurance of zero */
		if (offs)
			was = 1;
		
		if (was) {
			if (l >= size)
				return NULL;
				
			*(buff + l++) = *(char *)(digits + offs);
    } 
		v -= offs * div;
		div /= 10; 
	}
	if (!l) *(buff + l++) = *(char *)(digits + offs);
	*(buff + l) = 0;

	return buff;
}


char *std_itoah(uint_t i, char *buff, uint_t size, uint_t skip)
{
	uint_t l = 0;
	uint_t offs = 0;
	uint_t k, m, nz = 0;

	if (skip && !i) {
		buff[0] = '0';
		buff[1] = 0;
		return buff;
	}
		
	m = sizeof(int) * 8 - 4;	
	for (k = 0; k < sizeof(int) * 2; k++) {
		offs = ((i >> (m - 4 * k)) & 0xf);
		
		/* Skip leading zeros */
		if (skip && !offs && !nz)
			continue;
		nz = 1;
		if (l >= size) return NULL;
		
		*(buff + l++) = *(char *)(digitsh + offs);
  }
	*(buff + l) = 0;	
	return buff;
}


uint_t std_ahtoi(char *s)
{
	char *p;
	char was = 0;
	uint_t v = 0;
	unsigned char pow = 0;
	uint_t i;
	int k;

	for (k = std_strlen(s) - 1; k >= 0; k--) {
		p = (char *)(s + k);
		
		if ((!was) && ((*p == ' ') || (*p == '\t')))
			continue;
		was = 0;
		
		for (i = 0; i < 16; i++)
			if (digitsh[i] == *(char *)p) {
				was = 1;
				break;
			}
		if (!was) return 0;
		if (pow > 3) return v;
			
		v += (i << (pow * 4));
		pow++;
	}
	return v;
}


int std_strncmp(char *s1, char *s2, uint_t count)
{
	char *p;
	unsigned int k = 0;

	for (p = s1; *p; p++) {
		if ((*p != *(s2 + k)) && (k < count))
			return -1;
		k++;
	}

	if (k < count)
		return -1;

	return 0;
}


void std_putc(uint_t syn, char c)
{
	char s[2];
	
	s[0] = c;
	s[1] = (char)0;
	console_puts(syn, s);
}


void std_puts(uint_t syn, char *s)
{
	console_puts(syn, s);
}


void std_printf(char *fmt, ...)
{
	va_list ap;
	char *p;
	char buff[64];

	va_start(ap, fmt);
	
	for (p = fmt; *p; p++) {

		if (*p != '%') {
			if (*p == '\t')
				std_puts(CON_SYN, "  ");
			else
				std_putc(CON_SYN, *p);
			continue;
		}
				
		switch (*++p) {
		case 'd':
			std_puts(CON_SYN, std_itoa(va_arg(ap, int), buff, sizeof(buff)));
			break;
		case 'x':
			std_puts(CON_SYN, std_itoah(va_arg(ap, int), buff, sizeof(buff), 1));
			break;
		case 's':
			std_puts(CON_SYN, va_arg(ap, char *));
			break;
		case 'c':
			std_putc(CON_SYN, va_arg(ap, int));
			break;
		case 'p':
			std_puts(CON_SYN, std_itoah(va_arg(ap, int), buff, sizeof(buff), 0));
			break;
		case '%':
			std_putc(CON_SYN, '%');
			break;
		}
	}
	va_end(ap);
	return;
}
