/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Task execution
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

#include <hal/current/if.h>
#include <init/std.h>
#include <init/errors.h>
#include <task/elf.h>
#include <task/task.h>
#include <phfs/if.h>


/* Function loads Phoenix user program, creates new task and starts execution */
int exec(char *name)
{
	int h, err;
	u32 pos = 0;
	Elf32_Ehdr ehdr;
	Elf32_Phdr phdr;
	void *addr;
	uint_t k, npages, i;
	page_t *tp = NULL;
	vm_seg_t *seg;
	vm_map_t *map;
	uint_t flags = 0;
	
	if ((h = phfs_open(0, name, 0)) < 0) {
		std_printf("Can't open file '%s' [err=%p]!\n", name, h);
		return h;
	}

	/* Read header */
	if ((err = phfs_read(0, h, &pos, (u8 *)&ehdr, sizeof(ehdr))) < 0) {
		std_printf("Can't read file [err=%p]!\n", err);
		return err;
	}
		
	if ((ehdr.e_ident[0] != 0x7f) && (ehdr.e_ident[1] != 'E') &&
	    (ehdr.e_ident[2] != 'L') && (ehdr.e_ident[3] != 'F')) {
		std_printf("Bad executable!\n");
		return ERR_ARG;
	}
	
	/* Create virtual memory map for new task */
	if ((map = map_create()) == NULL) {
		return -1;
	}
	
	/* Load program sections */
	for (k = 0; k < ehdr.e_phnum; k++) {
	
		/* Read program header */
		pos = ehdr.e_phoff + k * sizeof(Elf32_Phdr);
		
		if ((err = phfs_read(0, h, &pos, (u8 *)&phdr, sizeof(phdr))) < 0) {
			std_printf("Can't read file [err=%p]!\n", err);
			return err;
		}

		if ((phdr.p_type == PT_LOAD) && (phdr.p_vaddr != 0)) {

			/* Alloc memory for the new program section */
			npages = (phdr.p_filesz % PAGE_SIZE) ? phdr.p_filesz / PAGE_SIZE + 1 : phdr.p_filesz / PAGE_SIZE;
			if ((tp = area_alloc(npages, REG_MEM)) == NULL) {
				map_free(map);
				return -1;
			}
			
			addr = (uchar_t *)(KERNEL_BASE + tp->num * PAGE_SIZE);
			
			/* Read data from file to memory */
			pos = phdr.p_offset;
			for (i = 0; i < phdr.p_filesz / (MSG_MAXLEN - 64); i++) {		
				
				if ((err = phfs_read(0, h, &pos, addr, MSG_MAXLEN - 64)) < 0) {
					std_printf("Can't read file [err=%p]!\n", err);
					return err;
				}
				
				addr += (MSG_MAXLEN - 64);
			}
			phfs_read(0, h, &pos, addr, phdr.p_filesz % (MSG_MAXLEN - 64));
 
			/* Create and map virtual memory segment */
			flags |= (PGHD_EXEC | PGHD_WRITE | PGHD_READ | PGHD_USER | PGHD_PRESENT);
			seg = seg_create(tp, (void *)phdr.p_vaddr, flags);

			if (!seg) {
				map_free(map);	
				return -1;
			}

			if (seg_map(map, seg)) {
				map_free(map);
				return -1;
			}
		
		}
	}

	/* Create new user task */
	create_task(name, map, (void *)ehdr.e_entry, 0);

	return 0;
}


/* exec (PSC) */
void psc_exec(char *name, int *err)
{
	*err = exec(name);
	return;
}
