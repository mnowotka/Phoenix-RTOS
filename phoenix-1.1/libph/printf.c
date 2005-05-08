/*
 * Phoenix-RTOS
 *
 * Standard library
 *
 * printf() and string manipulation routines
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

#include <libph.h>
#include <stdarg.h>
#include <arch/locore.h>


/*
 * string basic
 */


int ph_strlen(char *s)
{
	char *p;
	uint_t k = 0;

	for (p = s; *p; p++)
		k++;
	return k;
}


int ph_strncmp(char *s1, char *s2, uint_t count)
{
	char *p;
	unsigned int k = 0;

	for (p = s1; *p; p++) {
		if ((*p != *(s2 + k)) && (k < count)) return -1;
		k++;
	}

	if (k < count) return -1;

	return 0;
}


/*
 * conversion functions
 */


char *digits = "0123456789";
char *digitsh = "0123456789abcdef";


char *ph_itoa(int i, char *buff, uint_t size)
{
	int l;
	int div = 1000000000;
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

		/* If zero then set flag */
		if (offs) was = 1;
		
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


char *ph_itoah(uint_t i, char *buff, uint_t size)
{
	uint_t l = 0;
	uint_t offs = 0;
	uint_t k, m, nz = 0;
	
	m = sizeof(int) * 8 - 4;	
	for (k = 0; k < sizeof(int) * 2; k++) {
		offs = ((i >> (m - 4 * k)) & 0xf);
		
		/* Remove leading zeros */
		if (!offs && !nz)
			continue;
		nz = 1;
		if (l >= size) return NULL;
		
		*(buff + l++) = *(char *)(digitsh + offs);
  }
	*(buff + l) = 0;	
	return buff;
}


uint_t ph_ahtoi(char *s)
{
	char *p;
	char was = 0;
	uint_t v = 0;
	unsigned char pow = 0;
	uint_t i;
	int k;

	for (k = ph_strlen(s) - 1; k >= 0; k--) {
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
		if (pow > 7) return v;
			
		v += (i << (pow * 4));
		pow++;
	}
	return v;
}


uint_t ph_atoi(char *s)
{
	char *p;
	char was = 0;
	uint_t v = 0;
	uint_t pow = 0;
	int i, l, k;

	for (k = ph_strlen(s) - 1; k >= 0; k--) {
		p = (char *)(s + k);
		
		if ((!was) && ((*p == ' ') || (*p == '\t')))
			continue;
		was = 0;
		
		for (i = 0; i < 10; i++)
			if (digits[i] == *(char *)p) {
				was = 1;
				break;
			}
		if (!was)
			return 0;
		
		pow = 1;
		for (l = 1; l < ph_strlen(p); l++)
			pow *= 10;
		
		v += i * pow;
	}
	return v;
}


/*
 * output functions
 */


void ph_putc(char c)
{
	char s[2];
	
	s[0] = c;
	s[1] = 0;
	
	ph_devwrite(&dev_console, s, NULL, 2);
}


void ph_printf(char *fmt, ...)
{
	va_list ap;
	char *p;
	char buff[64];
	char *s;
	uint_t fldszpos = 0;
	uint_t fldsz, k;
	char fldszbuff[64];

	va_start(ap, fmt);
	
	for (p = fmt; *p; p++) {
		if (!fldszpos && (*p != '%')) {
			ph_putc(*p);
			continue;
		}
		if (!fldszpos && (*p == '%'))
			++p;
		
		switch (*p) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			fldszbuff[fldszpos++] = *p;
			break;
		
		case 'd':

			/* print padding if necessary */
			fldszbuff[fldszpos] = 0;
			ph_itoa(va_arg(ap, int), buff, sizeof(buff));
			if (fldszpos)
				fldsz = ph_atoi(fldszbuff);
			else
				fldsz = 0;				
			if (fldsz > ph_strlen(buff))
				for (k = 0; k < fldsz - ph_strlen(buff); k++)
					ph_putc(' ');
			
			/* print value */
			ph_devwrite(&dev_console, buff, NULL, ph_strlen(buff));
			fldszpos = 0;
			break;
			
		case 'x':
		
			/* print padding if necessary */			
			fldszbuff[fldszpos] = 0;
			ph_itoah(va_arg(ap, int), buff, sizeof(buff));
			if (fldszpos)
				fldsz = ph_atoi(fldszbuff);
			else
				fldsz = 0;
			
			if (fldsz > ph_strlen(buff))
				for (k = 0; k < fldsz - ph_strlen(buff); k++)
					ph_putc(' ');
			
			/* printf value */
			ph_devwrite(&dev_console, buff, NULL, ph_strlen(buff));
			fldszpos = 0;
			break;
			
		case 's':
			/* print padding if necessary */
			fldszbuff[fldszpos] = 0;
			s = va_arg(ap, char *);
			if (fldszpos)
				fldsz = ph_atoi(fldszbuff);
			else
				fldsz = 0;
	
			if (fldsz > ph_strlen(s))
				for (k = 0; k < fldsz - ph_strlen(s); k++)
					ph_putc(' ');
			
			/* print string */
			ph_devwrite(&dev_console, s, NULL, ph_strlen(s));
			fldszpos = 0;
			break;
			
		case 'c':
			ph_putc(va_arg(ap, int));
			fldszpos = 0;
			break;

		case '%':
			ph_putc('%');
			fldszpos = 0;
			break;
		}
	}
	va_end(ap);
	
	return;
}
