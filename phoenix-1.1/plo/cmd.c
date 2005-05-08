/* 
 * Phoenix-RTOS
 *
 * plo - operating system loader
 *
 * Loader commands
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
#include "msg.h"
#include "phfs.h"
#include "elf.h"


#define DEFAULT_BLANKS  " \t"
#define DEFAULT_CITES   "\""


void cmd_dump(char *s);
void cmd_go(char *s);
void cmd_help(char *s);
void cmd_load(char *s);
void cmd_memmap(char *s);
void cmd_cmd(char *s);
void cmd_timeout(char *s);
void cmd_save(char *s);


struct {
	void (*f)(char *);
	char *cmd;
	char *help;
} cmds[] = {
	{ cmd_dump,    "dump", "    - dumps memory, usage: dump <segment>:<offset>" },
	{ cmd_go,      "go!", "     - starts Phoenix-RTOS loaded into memory" },
	{ cmd_help,    "help", "    - prints this message" },
	{ cmd_load,    "load", "    - loads Phoenix-RTOS, usage: load [<boot device>]" },
	{ cmd_memmap,  "mem", "     - prints physical memory map" },
	{ cmd_cmd,     "cmd", "     - boot command, usage: cmd [<command>]" },
	{ cmd_timeout, "timeout", " - boot timeout, usage: timeout [<timeout>]" },
	{ cmd_save,    "save", "    - saves configuration" },	
	{ NULL, NULL, NULL }
};


char *devices[] = { "com1", "com2", NULL };


/* Function prints progress indicator */
void cmd_progress(u32 p)
{
	char *states = "-\\|/"; 
	
	plostd_printf(ATTR_LOADER, "%c%c", 8, states[p % plostd_strlen(states)]);
	return;
}


/* Function skips blank characters */
void cmd_skipblanks(char *line, unsigned int *pos, char *blanks)
{
	char c, blfl;
	unsigned int i;
	
	while ((c = *((char *)(line + *pos))) != 0) {
		blfl = 0;
		for (i = 0; i < plostd_strlen(blanks); i++) {
			if (c == *(char *)(blanks + i)) {
				blfl = 1;
				break;
			}
		}
		if (!blfl)
			break;
		(*pos)++;
	}
	return;
}


/* Function retrieves next symbol from line */
char *cmd_getnext(char *line, unsigned int *pos, char *blanks, char *cites, char *word, unsigned int len)
{
	char citefl = 0, c;
	unsigned int i, wp = 0;

	/* Skip leading blank characters */
	cmd_skipblanks(line, pos, blanks);

	wp = 0;
	while ((c = *(char *)(line + *pos)) != 0) {
		
		/* Test cite characters */
		if (cites) {
			for (i = 0; cites[i]; i++) {
				if (c != cites[i])
					continue;
				citefl ^= 1;
				break;
			}
			
			/* Go to next iteration if cite character found */
			if (cites[i]) {
				(*pos)++;				
				continue;
			}
		}
				
		/* Test separators */
		for (i = 0; blanks[i]; i++) {
			if (c != blanks[i])
				continue;
			break;
		}		
		if (!citefl && blanks[i])
			break;

		word[wp++] = c;
		if (wp == len)
			return NULL;
	
		(*pos)++;
	}
	
	if (citefl)
		return NULL;

	word[wp] = 0;	
	return word;
}


void cmd_dump(char *s)
{
	char word[LINESZ + 1];
	unsigned int p = 0;
	int xsize = 16;
	int ysize = 16;
	unsigned int x, y;
	u16 segm, offs, isegm, ioffs;
	u8 b;

	/* Get address */
	cmd_skipblanks(s, &p, DEFAULT_BLANKS);
	
	if (cmd_getnext(s, &p, ":", NULL, word, sizeof(word)) == NULL) {
		plostd_printf(ATTR_ERROR, "\nSize error!\n");
		return;
	}
	if (*word == 0) {
		plostd_printf(ATTR_ERROR, "\nBad segment!\n");
		return;
	}
	segm = plostd_ahtoi(word);
	
	/* Skip colon */
	cmd_skipblanks(s, &p, DEFAULT_BLANKS ":");

	if (cmd_getnext(s, &p, DEFAULT_BLANKS, NULL, word, sizeof(word)) == NULL) {
		plostd_printf(ATTR_ERROR, "\nSize error!\n");
		return;
	}
	if (*word == 0) {
		plostd_printf(ATTR_ERROR, "\nBad offset!\n");
		return;
	}	
	offs = plostd_ahtoi(word);
	
	plostd_printf(ATTR_LOADER, "\n");
	plostd_printf(ATTR_LOADER, "Memory dump from %p:%p\n", segm, offs);
	plostd_printf(ATTR_LOADER, "--------------------------\n");

	for (y = 0; y < ysize; y++) {
		plostd_printf(ATTR_LOADER, "%p:%p   ", segm, offs);
		isegm = segm;
		ioffs = offs;

		/* Print byte values */
		for (x = 0; x < xsize; x++) {
			low_copyfrom(segm, offs, &b, 1);
			if (b & 0xf0)
				plostd_printf(ATTR_LOADER, "%x ", b);
			else
				plostd_printf(ATTR_LOADER, "0%x ", b);
			
			if (offs++ == 0xffff)
				segm++;
		}
		plostd_printf(ATTR_LOADER, "  ");

		/* Print ASCII representation */
		segm = isegm;
		offs = ioffs;

		for (x = 0; x < xsize; x++) {
			low_copyfrom(segm, offs, &b, 1);
			if ((b <= 32) || (b > 127))
				plostd_printf(ATTR_LOADER, ".", b);
			else
				plostd_printf(ATTR_LOADER, "%c", b);
			
			if (offs++ == 0xffff)
				segm++;
		}
		plostd_printf(ATTR_LOADER, "\n");
	}
	return;
}


void cmd_go(char *s)
{
	plostd_printf(ATTR_LOADER, "\nStarting Phoenix-RTOS...\n");
	low_launch();
	return;
}


void cmd_help(char *s)
{
	int k;

	plostd_printf(ATTR_LOADER, "\n");
	plostd_printf(ATTR_LOADER, "Loader commands:\n");
	plostd_printf(ATTR_LOADER, "----------------\n");

	for (k = 0; cmds[k].cmd; k++)
		plostd_printf(ATTR_LOADER, "%s %s\n", cmds[k].cmd, cmds[k].help);
	return;
}


void cmd_loadkernel(unsigned int dn, char *arg)
{
	char *path;
	s32 h;
	u32 p;
	Elf32_Ehdr hdr;
	Elf32_Phdr phdr;
	Elf32_Word i, k;
	Elf32_Half seg, offs;
	Elf32_Word size, l;
	u8 buff[384];
	int err;
	
	plostd_printf(ATTR_LOADER, "\n");

	path = arg ? arg : KERNEL_PATH;
	if ((h = phfs_open(dn, path, 0)) < 0) {
		plostd_printf(ATTR_ERROR, "Kernel not found!\n");
		return;
	}

	/* Read ELF header */
	p = 0;
	if (phfs_read(dn, h, &p, (u8 *)&hdr, (u32)sizeof(Elf32_Ehdr)) < 0) {
		plostd_printf(ATTR_ERROR, "Can't read ELF header!\n");
		return;
	}
	if ((hdr.e_ident[0] != 0x7f) && (hdr.e_ident[1] != 'E') &&
			(hdr.e_ident[2] != 'L') && (hdr.e_ident[3] != 'F')) {
		plostd_printf(ATTR_ERROR, "Kernel file isn't ELF object!\n");
		return;
	}

	/* Read program segments */
	for (k = 0; k < hdr.e_phnum; k++) {
		p = hdr.e_phoff + k * sizeof(Elf32_Phdr);
		if (phfs_read(dn, h, &p, (u8 *)&phdr, (u32)sizeof(Elf32_Phdr)) < 0) {
			plostd_printf(ATTR_ERROR, "Can't read Elf32_Phdr, k=%d!\n", k);
			return;
		}
 
		if ((phdr.p_type == PT_LOAD) && (phdr.p_vaddr != 0)) {
			seg = (phdr.p_vaddr - KERNEL_BASE) / 16;
			offs = (phdr.p_vaddr - KERNEL_BASE) % 16;

			plostd_printf(ATTR_LOADER, "Reading segment %p:%p:  ", seg, offs);
			
			for (i = 0; i < phdr.p_filesz / sizeof(buff); i++) {
				p = phdr.p_offset + i * sizeof(buff);
				if ((err = phfs_read(dn, h, &p, buff, (u32)sizeof(buff))) < 0) {
					plostd_printf(ATTR_ERROR, "\nCan't read segment data, k=%d, err=%d!\n", k, err);
					return;
				}
				low_copyto(seg, offs, buff, sizeof(buff));
				offs += sizeof(buff);
				cmd_progress(i);
			}
			
			/* Last segment part */
			size = phdr.p_filesz % sizeof(buff);
			if (size != 0) {
				p = phdr.p_offset + i * sizeof(buff);
				if (phfs_read(dn, h, &p, buff, size) < 0) {
					plostd_printf(ATTR_ERROR, "\nCan't read segment data, k=%d!\n", k);
					return;
				}
			}
			low_copyto(seg, offs, buff, size);
			cmd_progress(i);
			plostd_printf(ATTR_LOADER, "%c[ok]\n", 8);
		}
	}
	return;
}


void cmd_load(char *s)
{
	char word[LINESZ + 1];
	unsigned int p = 0;
	unsigned int dn;
	
	cmd_skipblanks(s, &p, DEFAULT_BLANKS);	
	if (cmd_getnext(s, &p, DEFAULT_BLANKS, NULL, word, sizeof(word)) == NULL) {
		plostd_printf(ATTR_ERROR, "\nSize error!\n");
		return;
	}
	
	/* Show boot devices if parameter is empty */
	if (*word == 0) {
		plostd_printf(ATTR_LOADER, "\nBoot devices: ");	
		for (dn = 0; devices[dn]; dn++)
			plostd_printf(ATTR_LOADER, "%s ", devices[dn]);
		plostd_printf(ATTR_LOADER, "\n");
		return;
	}
	
	for (dn = 0; devices[dn]; dn++)  {
		if (!plostd_strcmp(word, devices[dn]))
			break;
	}	
	if (!devices[dn]) {
		plostd_printf(ATTR_ERROR, "\n'%s' - unknown boot device!\n", word);
		return;
	}
	cmd_loadkernel(dn, NULL);
	return;
}


void cmd_memmap(char *s)
{
	int k, i;
	int id;
	char *stat;
	low_mmitem_t mmitem;

	plostd_printf(ATTR_LOADER, "\n");
	plostd_printf(ATTR_LOADER, "BIOS e820 memory map\n");
	plostd_printf(ATTR_LOADER, "--------------------\n");

	for (k = 0; k < MM_MAXSZ; k++) {
		if (low_mmget(k, &mmitem) == ERR_LOW_MMSIZE)
			break;
		
		if (mmitem.attr == 1)
			stat = "free";
		else
			stat = "reserved";
		plostd_printf(ATTR_LOADER, "%p%p  +%p%p  -  %s\n", mmitem.addr_hi, mmitem.addr_lo,
		              mmitem.len_hi, mmitem.len_lo, stat);
	}
	
	plostd_printf(ATTR_LOADER, "\n");
	plostd_printf(ATTR_LOADER, "plo memory map\n");
	plostd_printf(ATTR_LOADER, "--------------\n");
	plostd_printf(ATTR_LOADER, "0000%p  +00001000  -  BIOS variables\n", 0);
	plostd_printf(ATTR_LOADER, "0000%p  +0000%p  -  gdt[]\n", GDT_SEG << 4, GDT_SIZE);
	plostd_printf(ATTR_LOADER, "0000%p  +0000%p  -  idt[]\n", IDT_SEG << 4, IDT_SIZE);
	plostd_printf(ATTR_LOADER, "0000%p  +0000%p  -  syspage\n", SYSPAGE_SEG << 4, SYSPAGE_SIZE);
	plostd_printf(ATTR_LOADER, "0000%p  +0000%p  -  kernel stack\n", (SYSPAGE_SEG << 4) + 0x1000, INIT_ESP - (SYSPAGE_SEG << 4) - 0x1000);
	plostd_printf(ATTR_LOADER, "0000%p  +00010000  -  plo\n", 0x7c00);
	plostd_printf(ATTR_LOADER, "0001%p  +0008%p  -  free\n", 0x7c00, 0xfc00 - 0x7c00);
	return;
}


/* Function setups boot command */
void cmd_cmd(char *s)
{
	unsigned int p = 0;
	int l;
	
	plostd_printf(ATTR_LOADER, "\n");
	cmd_skipblanks(s, &p, DEFAULT_BLANKS);
	s += p;
	
	if (*s) {
		low_memcpy(_plo_command, s, l = min(plostd_strlen(s), CMD_SIZE - 1));
		*((char *)_plo_command + l) = 0;
	}

	plostd_printf(ATTR_LOADER, "cmd=%s\n", (char *)_plo_command);
	
	return;
}


/* Function setups boot timeout */
void cmd_timeout(char *s)
{
	char word[LINESZ + 1];
	unsigned int p = 0;
	
	plostd_printf(ATTR_LOADER, "\n");
	cmd_skipblanks(s, &p, DEFAULT_BLANKS);
	
	if (cmd_getnext(s, &p, DEFAULT_BLANKS, DEFAULT_CITES, word, sizeof(word)) == NULL) {
		plostd_printf(ATTR_ERROR, "Syntax error!\n");
		return;
	}
	if (*word)
		_plo_timeout = plostd_ahtoi(word);
	
	plostd_printf(ATTR_LOADER, "timeout=0x%x\n", _plo_timeout);	
	return;
}


/* Function saves boot configuration */
void cmd_save(char *s)
{
	u16 err;
	
	plostd_printf(ATTR_LOADER, "\n");
	if (err = _plo_save())
		plostd_printf(ATTR_ERROR, "Can't save configuration [err=0x%x]!\n", err);
	else
		plostd_printf(ATTR_LOADER, "Configuration saved\n");
	
	return;
}


/* Function parses loader commands */
void cmd_parse(char *line)
{
	int size, k;
	char word[LINESZ + 1], cmd[LINESZ + 1];
	unsigned int p = 0, wp;
	
	for (;;) {		
		if (cmd_getnext(line, &p, ";", DEFAULT_CITES, word, sizeof(word)) == NULL) {
			plostd_printf(ATTR_ERROR, "\nSyntax error!\n");
			return;
		}
		if (*word == 0)
			break;
		
		wp = 0;
		cmd_skipblanks(word, &wp, DEFAULT_BLANKS);
		if (cmd_getnext(word, &wp, DEFAULT_BLANKS, DEFAULT_CITES, cmd, sizeof(cmd)) == NULL) {
			plostd_printf(ATTR_ERROR, "\nSyntax error!\n");
			return;
		}
				
		/* Find command and launch associated function */
		for (k = 0; cmds[k].cmd != NULL; k++) {
			if (!plostd_strcmp(cmd, cmds[k].cmd)) {
				cmds[k].f(word + wp);
				break;
			}
		}			
		if (!cmds[k].cmd)
			plostd_printf(ATTR_ERROR, "\n'%s' - unknown command!\n", cmd);
	}
		
	return;
}

