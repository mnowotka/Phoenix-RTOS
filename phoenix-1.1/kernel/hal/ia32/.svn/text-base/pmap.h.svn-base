/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * pmap interface - machine dependent part of VM subsystem
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

#ifndef _PMAP_H_
#define _PMAP_H_

#include <hal/current/types.h>


/* Architecure dependent page attributes */
#define PGHD_PRESENT  0x01
#define PGHD_SYSTEM   0x00
#define PGHD_USER     0x04
#define PGHD_READ     0x00
#define PGHD_WRITE    0x02
#define PGHD_EXEC     0x00
#define PGHD_NOEXEC   0x00


/* Architecure dependent page table attributes */
#define PTHD_PRESENT  0x01
#define PTHD_SYSTEM   0x00
#define PTHD_USER     0x04
#define PTHD_WRITE    0x02
#define PTHD_READ     0x00


/* Page attributes used by functions allocating memory areas */
#define PG_DMA       0x80000000   /* {31} page can be used for DMA */
#define PG_RSVD      0x40000000   /* {30} page is reserved */
#define PG_KERNEL    0x20000000   /* {29} page is used by the kernel */
#define PG_PRESENT   0x10000000   /* {28} page is present - reserved for future */


/* Testing macros */
#define IS_DMA(pf)    ((pf >> 31) & 1)
#define IS_FREE(pf)   (!((pf >> 30) & 1))
#define IS_KERNEL(pf) ((pf >> 29) & 1)


/* Page descriptor used by other parts of VM subsystem */
typedef struct _page_t {
	struct _page_t *next;       /* next page in area */
	struct _page_t *prev;       /* previous page */
	uint_t         flags;       /* atrybuty strony i przenaczenie */
	uint_t         num;         /* numer strony fizycznej */
} page_t;


/* Machine dependent interface, used to emulate one-level page table */
typedef struct _pmap_t {
	uint_t npages;       /* total mapped pages */
	void *pdir;          /* addres of the page directory */
} pmap_t;


/*
 * Function initializes machine dependent pmap interface. In initialization
 * procedure next kernel page tables are created (neccesary to map all physical memory
 * in kernel space). Top of the staticaly allocated kernel memory is increased.
 * num_of_ptable = 1 GB / 4 MB = 256
 * kernel_mem_top += size / PAGE_DIR_SIZE / PAGE_SIZE 
 */
extern void pmap_init(uint_t size);


/* Function fills page_t structure for page frame given by pn */
extern page_t *pmap_getphyspage(uint_t size, page_t *page, uint_t pn);


/* Function returns top of statically allocated kernel memory */
extern uint_t pmap_get_kmem_top(void);


/* Function creates empty pmap structure */
extern pmap_t *pmap_create(void);


/* Functions maps page at specified address */
extern int pmap_map(pmap_t *pmap, page_t *page, void *vaddr, uint_t flags);


/* Functions releases pmap structure */
extern void pmap_free(pmap_t *pmap);


#endif
