/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Virtual memory management
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

#ifndef _VM_H_
#define _VM_H_

#include <hal/current/types.h>
#include <hal/current/pmap.h>


#define DMA_MEM     0  /* DMA memory allocation flag */
#define REG_MEM     1  /* Regular memory allocation flag */
#define KERNEL_MEM  2  /* Kernel memory allocation flag */


/* Structure defines linear memory map describing all pages */
typedef struct _mem_map_t {
	mutex_t mutex;         /* access mutex */
	uint_t size;           /* number of pages available in the system */
	page_t *first_page;    /* first page descriptor */
	uint_t total_free;     /* memory statistics... */
  uint_t kernel_mem;
	uint_t dma_free;
}	mem_map_t;


/* Memory info structure (used in system call) */
typedef struct _meminfo_t {
	uint_t total;              /* physical memory size */
	uint_t total_free;         /* free memory */
  uint_t kernel_rsvd;        /* kernel reserved memory */
	uint_t dma_free;           /* free memory available for DMA */
}	meminfo_t;


/* Structure describes segment of process virtual space */
typedef struct _vm_seg_t {
	uint_t flags;            /* protection attributes */
	void *vaddr;             /* starting virtual address */
	uint_t size;             /* segment size */
	page_t *pages;           /* area pages */
	struct _vm_seg_t *next;
	struct _vm_seg_t *prev;
} vm_seg_t;


/* Memory map of tasks virtual memory */
typedef struct _vm_map_t {
	pmap_t *pmap;            /* on-levele page table implemented by pmap interface */
	vm_seg_t *segs;          /* task memory segments - double linked list */
} vm_map_t;


/* Function creates memory map obtaining description from pmap_get_page() */
extern void init_mem_map(uint_t size);


/* Function allocates area (list of pages) */
extern page_t *area_alloc(uint_t size, uint_t dest);


/* Function releases page list */
extern void area_free(page_t *page);


/* Function prints memory usage statistics */
extern void disp_meminfo(void);


/* Function returns memory usage statistics (PSC) */
extern void get_meminfo(meminfo_t *mi);


/* Functions prints information about allocated pages */
extern void disp_areainfo(page_t *page);


/* Inline function which allocates pages for kernel */
static inline void *kernel_pages_alloc(uint_t n)
{
	page_t *p = area_alloc(n, KERNEL_MEM);
	void *addr;
	
	addr = p ? (void *)(p->num * PAGE_SIZE + KERNEL_BASE) : (void *)0;
	return addr;
}


/* Function releases pages allocated for kernel */
extern void kernel_pages_free(void *addr);


/* Function creates virtual memory map */
extern vm_map_t *map_create(void);


/* Function creates virtual memory segment */
extern vm_seg_t *seg_create(page_t *pages, void *vaddr, uint_t flags);


/* Function maps segment to tasks virtual space */
extern int seg_map(vm_map_t *map, vm_seg_t *seg);


/* Function releases task segments */
extern void release_segs(vm_map_t *map);


/* Functions releases virtual memory map */
extern void map_free(vm_map_t *map);


#endif
