/*
 * Phoenix-RTOS
 *
 * Trivial shell
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

#include "lex.h"


#define PROMPT_SIZE       1024

#define max(a, b) (a > b ? a : b)


/* Function displays list of currently running tasks */
void do_ps(char *line, uint_t *lpos, char *word, uint_t word_size)
{
	char *states[] = { "run", "slp", "rdy", "strt", "cwt", "zmb" };
	uint_t pids[1024];
	uint_t ntasks, k;
	taskinfo_t ti;
	
	ph_printf("%4s %10s %5s %5s %6s\n", "PID", "NAME", "STATE", "PRTY", "PPID"); 
	ph_gettasks(pids, 128, &ntasks);
	
	for (k = 0; k < ntasks; k++) {
		ph_gettaskinfo(pids[ntasks - k - 1], &ti);
		if (ti.type)
			ph_printf("%4d %10s %5s %5d %6d\n", ti.id, ti.name, states[ti.state], ti.priority, ti.ppid);
		else
			ph_printf("%4d %9s+ %5s %5d %6d\n", ti.id, ti.name, states[ti.state], ti.priority, ti.ppid);
	}
	return;
}


/* Function prints system memory statistics */
void do_mi(char *line, uint_t *lpos, char *word, uint_t word_size)
{
	meminfo_t mi;
	
	ph_printf("%10s %10s %10s %10s\n", "TOTAL", "KRNL_RSVD", "DMA_FREE", "FREE"); 
	ph_getmeminfo(&mi);
	ph_printf("%10d %10d %10d %10d\n", mi.total >> 10, mi.kernel_rsvd >> 10, mi.dma_free >> 10, mi.total_free >> 10);
	return;
}


/* Function sends signal to specified task */
void do_raise(char *line, uint_t *lpos, char *word, uint_t word_size)
{
	uint_t sig;
	uint_t pid;
	char *sigs[2] = { "CHLD", "KILL" };
	uint_t was = 0;
	
	if ((word = getnextsym(line, lpos, word, word_size)) == NULL) {
		ph_printf("Bad syntax!, usage: raise <signal> <pid>\n");
		return;
	}
	
	for (sig = 0; sig < 2; sig++)
		if (!ph_strncmp(sigs[sig], word, ph_strlen(word))) {
			was = 1;
			break;
		}
		
	/* If can' translate word to number convert it by using ph_atoi() function */ 
	if (!was)
		sig = ph_atoi(word);
		
	if ((word = getnextsym(line, lpos, word, word_size)) == NULL) {
		ph_printf("Bad syntax!, usage: raise <signal> <pid>\n");
		return;
	}
	
	pid = ph_atoi(word);	
	ph_raise(pid, sig);
	return;
}


/* Function prints help information */
void do_help(char *line, uint_t *lpos, char *word, uint_t word_size)
{
	ph_printf("-\\- Phoenix shell, ver. 1.1, (c) Pawel Pisarczyk, 2000-2001\n");
	return;
}


/* Function exits from shell */
void do_exit(char *line, uint_t *lpos, char *word, uint_t word_size)
{
	ph_exit(0);
	return;
}


/* Function injects fault at given address */
void do_inject(char *line, uint_t *lpos, char *word, uint_t word_size)
{
	uint_t addr, mask, op;
	
	if ((word = getnextsym(line, lpos, word, word_size)) == NULL) {
		ph_printf("Bad syntax!, usage: inject <addr> <bit> <type>\n");
		return;
	}	
	addr = ph_ahtoi(word);
		
	if ((word = getnextsym(line, lpos, word, word_size)) == NULL) {
		ph_printf("Bad syntax!, usage: inject <addr> <bit> <type>\n");
		return;
	}	
	mask = ph_ahtoi(word);
	
	if ((word = getnextsym(line, lpos, word, word_size)) == NULL) {
		ph_printf("Bad syntax!, usage: inject <addr> <bit> <type>\n");
		return;
	}	
	op = ph_ahtoi(word);
	
	ph_inject((void *)addr, mask, op);
	return;
}


#ifdef USE_RAMDISK

/* Function prints list of files on ramdisk */
void do_ls(char *line, uint_t *lpos, char *word, uint_t word_size)
{
	ramdisk_t rd;
	uint_t k;
	
	if (getramdiskinfo(&rd) < 0) {
		printf("Ramdisk isn't loaded!\n");
		return;
	}
	
	for (k = 0; k < RAMDISK_MAX_FILES; k++) {
		if (*rd.files[k].name)
			printf("%12s, offs=%d\n", rd.files[k].name, rd.files[k].offset);
	}
	return;
}

#endif


struct {
	char *s;
	void (*cmnd)(char *, uint_t *, char *, uint_t);
}

cmnds[] = {
	{ "ps", &do_ps },
	{ "mi", &do_mi },
	{ "raise", &do_raise },
	{ "help", &do_help },
	{ "exit", &do_exit },
	{ "inject", &do_inject },
	{ NULL, NULL }
};


void _start(void)
{
	char line[PROMPT_SIZE];
	char word[PROMPT_SIZE];
	char c;
	uint_t pos, err, lpos, k;
	int exit_code;
	uint_t pid, iscmnd, bg = 0;

	for (;;) {
		ph_printf("phoenix%% ");
		
		pos = 0;
		err = 0;
		do {
			ph_devread(&dev_console, &c, NULL, 1);
			if (c == '\177') {
				if (pos) {
					line[--pos] = 0;
					ph_printf("%c", c);
				}
				continue;
			}
			else
				ph_printf("%c", c);
			
			line[pos++] = c;
			if (pos >= PROMPT_SIZE) {
				ph_printf("ERROR, To many chars!\n");
				err = 1;
				break;
			}
		} while (c != '\r');		
		line[pos] = 0;
		
		/* Parse command line */
		if (!err) {
			lpos = 0;
			if (!getnextsym(line, &lpos, word, PROMPT_SIZE))
				continue;			
			iscmnd = 0;	
			for (k = 0; cmnds[k].s; k++) {
				if (!ph_strncmp(word, cmnds[k].s, max(ph_strlen(cmnds[k].s), ph_strlen(word)))) {
					cmnds[k].cmnd(line, &lpos, word, PROMPT_SIZE);
					iscmnd = 1;
					break;
				}
			}
			
			if (iscmnd)
				continue;
				
			/* If no command recognized try to execute program */
			if (pos >= 2) {
				line[pos - 1] = 0;
				
				if (*(char *)(word + ph_strlen(word) - 1) == '&') {
					*(word + ph_strlen(word) - 1) = 0;
					bg = 1;
				} 
				if (ph_exec(word) < 0) {
					ph_printf("phoenix: Can't execute %s\n", line);
					continue;
				}
				
				if (!bg) {	
					pid = ph_wait(&exit_code);
					if (exit_code < 0)
						ph_printf("Task [%d] terminated with code %d.\n", pid, exit_code);
				}
				bg = 0;
				continue;
			}
		}
	}
	return;
}
