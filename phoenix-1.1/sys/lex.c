/*
 * Phoenix-RTOS
 *
 * Simple lexical scanner
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

#include "lex.h"
#include <arch/locore.h>


/*
 * Character tables for the scanner
 */

/* Default values */
#define DEF_BLANK	" \t"                 /* blank chars */
#define DEF_CITE	"\"\'"                /* cite chars */
#define DEF_SEP		" \t\r\n,"            /* separators */           
#define DEF_TERM	'\n'                  /* terminators */


char *blanks = DEF_BLANK;
char *cites  = DEF_CITE;
char *seps   = DEF_SEP;
char terms   = DEF_TERM;


/*
 * Function initializes parser.
 */
void lexinit(char *blank, char *cite, char *sep, char term)
{
	blanks	= blank;
	cites	= cite;
	seps	= sep;
	terms	= term;
	return;
}


/*
 * Functions skips blanks from the current position.
 */
int skip_blanks(char *line, unsigned int *pos)
{
	
	char c;
	char wasbl;
	unsigned int i;
	
	/* Pominiecie wiodacych znakow bialych */
	while ((c = *((char *)(line + *pos))) != 0) {
		wasbl = 0;
		for (i = 0; i < ph_strlen(blanks); i++) {
			if (c == *(char *)(blanks + i)) {
				wasbl = 1;
				break;
			}
		}
		if (wasbl) {
			(*pos)++;
			continue;
		}
		else
			return 0;
	}

	return -1;
}


/*
 * Function gets next symbol in line starting for position pointed by pos.
 * Symbol is stored in the buffer pointed by word.
 */
char *getnextsym(char *line, unsigned int *pos, char *word, unsigned int wsize)
{
	char was_cite = 0;
	char was_word = 0;
	unsigned int start_pos;
	unsigned int end_pos = 0;
	unsigned int i;
	unsigned int word_size;
	char c;
		
	if (skip_blanks(line, pos) < 0)
		return NULL;
	
	start_pos = *pos;
	while ((c = *(char *)(line + *pos)) != 0) {
		
		/* Test if c is cite character */	
		for (i = 0; i < ph_strlen(cites); i++) {
			if (c == *(char *)(cites + i)) {
				if (!was_cite) {
					was_cite = 1;
					if (*pos > start_pos) 
						return NULL;
					start_pos++;
					break;
				}
				else {
					was_cite = 0;
					end_pos = *pos;
					was_word = 1;
					(*pos)++;
					break;
				}
			}
		}

		if (was_word) break;

		/* Test if c is separator. */
		for (i = 0; i < ph_strlen(seps); i++) {
			if (c == *((char *)(seps + i))) {
				if (was_cite)
					break;
				else {
					end_pos = *pos;
					
					/* If separator isn't blank return it as the next symbol */
					if (end_pos - start_pos == 0) {
						end_pos++;
						(*pos)++;
					}
					was_word = 1;
					break;
				}
			}
		}

		if (was_word)
			break;

		(*pos)++;
	}

	if (!was_cite && !was_word)
		end_pos = *pos;

	if ((was_word) || (!was_cite)) {
		word_size = end_pos - start_pos;

		if (word_size + 1 > wsize)
			return NULL;

		__memcpy(word, (char *)(line + start_pos), word_size);
		*(char *)(word + word_size) = 0;
		return word;
	}
	
	return NULL;
}
