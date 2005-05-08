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

#include <hal/current/types.h>
#include <hal/current/defs.h>
#include <hal/current/locore.h>
#include <hal/current/pmap.h>
#include <init/std.h>
#include <vm/vm.h>
#include <vm/kmalloc.h>


/* Global memory map */
mem_map_t mem_map;

/* Size of physical memory - variable is set by bootstrap code */
uint_t physmem_size = 0;


/* Function creates memory map obtaining description from pmap_get_page() */
void init_mem_map(uint_t size)
{
	uint_t k;
	page_t *page;
	
	/* Memory map mutex initialization */
	unlock(&mem_map.mutex);
	
	/*
	 * Memory map begins from end of the statically allocated kernel memory pointed
	 * by kmem_top. Concrete value of kmem_top is calculated by pmap_init() function
	 * and depends on physical memory size. It is supposed that maximum size of
	 * physical memory handled by Phoenix is 1 GB. This will be changed in
	 * near future. 
	 */
	mem_map.first_page = (page_t *)(KERNEL_BASE + pmap_get_kmem_top());

	for (k = 0; k < size / PAGE_SIZE; k ++) {
		page = pmap_getphyspage(size, mem_map.first_page + k, k);
		
		if (IS_KERNEL(page->flags)) {
			mem_map.kernel_mem++;
		}
		
		if (IS_FREE(page->flags))
			mem_map.total_free++;

		if (IS_DMA(page->flags))
			mem_map.dma_free++;
	}	
	mem_map.size = k;
	
	return;
}


/* Function finds coherent area starting from address given by page */
static inline page_t *find_coherent(uint_t size, page_t *page, uint_t spn, uint_t dest)
{
	uint_t i;
	page_t *tpage;
	
	for (i = 1; i < size; i++) {
		if (spn + i >= mem_map.size)
			return NULL;
		
		tpage = page + i;
			 		
		/* DMA allocation */
		if (dest == DMA_MEM) {
		
			/* There is no DMA page */
			if ((!IS_FREE(tpage->flags)) || (!IS_DMA(tpage->flags)))
				return NULL;
		}
		
		/* Kernel allocation */
		else {
			if (!IS_FREE(tpage->flags))
				return NULL;
		}
		
		/* Add to list */
		tpage->prev = tpage - 1;
		tpage->prev->next = tpage;
		tpage->next = NULL;		
	}
		
	/* If memory is allocated mark all pages reserved, and decrease counters */
	for (i = 0; i < size; i++) {
		if (IS_DMA((page + i)->flags)) mem_map.dma_free--;
		(page + i)->flags |= PG_RSVD;
	}
	
	return page;
}


/* Function marks pages as reserved */
static inline void alloc_pages(page_t *page)
{
	page_t *temp;
	
	temp = page;	
	do {
		temp->flags |= PG_RSVD;
		
		if (IS_DMA(temp->flags))
			mem_map.dma_free--;

		temp = temp->next;
	} while (temp != NULL);
	return;
}


/* Function marks pages as free */
static inline void free_pages(page_t *page)
{
	page_t *temp;
	
	temp = page;
		
	do {
		temp->flags &= (~PG_RSVD);
				
		/* Update statistics */
		if (IS_DMA(temp->flags))
			mem_map.dma_free++;

		mem_map.total_free++;
		
		temp = temp->next;
	} while (temp != NULL);
	
	return;
}


/* Function allocates area (list of pages) */
page_t *area_alloc(uint_t size, uint_t dest)
{
	uint_t k;
	page_t *page, *tpage;
	page_t *res;
	page_t *last = NULL;
	char found = 0;
	uint_t cnt = 0;
		
	lock(&mem_map.mutex);
	
	if (mem_map.total_free < size) {
		unlock(&mem_map.mutex);
		return NULL;
	}
	
	page = mem_map.first_page + 1;
	
	/* DMA allocation */
	if (dest == DMA_MEM) {
		for (k = 1; k < mem_map.size; k++) {
			 
			/* Find first page for DMA */
			if ((IS_FREE(page->flags)) && (IS_DMA(page->flags))) {
				page->prev = NULL;
				page->next = NULL;
				
				/* Test next pages */
				if (find_coherent(size, page, k, dest) != NULL) {
					found = 1;
					break;
				}
			}
			page++;
		}
	}
	
	/* Kernel allocation - area must be physically coherent */
	if (dest == KERNEL_MEM) {
		for (k = 1; k < mem_map.size; k++) {			 
			if (IS_FREE(page->flags)) {
				page->prev = NULL;
				page->next = NULL;
				
				if (find_coherent(size, page, k, dest) != NULL) {
					found = 1;
					break;
				}
			}
			page++;
		}
	}
		
	/* Regular allocation */
	if (dest == REG_MEM) {
		tpage = page;
		for (k = 1; k < mem_map.size; k++) {
			
			if (IS_FREE(tpage->flags)) {
				if (last == NULL) {
					tpage->prev = NULL;
					tpage->next = NULL;
					last = tpage;
					
					page = tpage;
					cnt++;
				}	
				else {
					tpage->prev = last;
					tpage->prev->next = tpage;
					tpage->next = NULL;
					last = tpage;
					cnt++;
				}
			}
			
			/* If area found set flag */
			if (cnt == size) {
				alloc_pages(page);
				found = 1;
				break;
			}
			tpage++;
		}
	}
	
	/* If area found update statistics */
	if (found) {
		mem_map.total_free -= size;
		res = page;
	}
	else
		res = NULL;
	
	unlock(&mem_map.mutex);
	return res;
}


/* Function releases page list */
void area_free(page_t *page)
{
	lock(&mem_map.mutex);
	free_pages(page);			
	unlock(&mem_map.mutex);
	return;
}


/* Function releases pages allocated for kernel */
void kernel_pages_free(void *addr)
{
	page_t *p = mem_map.first_page + (uint_t)(addr - KERNEL_BASE) / PAGE_SIZE;
	
	area_free(p);
	return;
}


/* Function prints memory usage statistics */
void disp_meminfo(void)
{
	lock(&mem_map.mutex);
	
	std_printf("meminfo: total free: %d KB, kernel rsvd: %d KB, dma free: %d KB\n",
	           mem_map.total_free * 4, mem_map.kernel_mem * 4, mem_map.dma_free * 4);
	
	unlock(&mem_map.mutex);
	return;
}


/* Function returns memory usage statistics (PSC) */
void get_meminfo(meminfo_t *mi)
{
	lock(&mem_map.mutex);	
	mi->total = mem_map.size * PAGE_SIZE;
	mi->total_free = mem_map.total_free * PAGE_SIZE;
	mi->kernel_rsvd = mem_map.kernel_mem * PAGE_SIZE;
	mi->dma_free = mem_map.dma_free * PAGE_SIZE;
	unlock(&mem_map.mutex);
	return;
}


/* Functions prints information about allocated pages */
void disp_areainfo(page_t *page)
{
	page_t *temp;
	
	temp = page;
	while (temp != NULL) {
		std_printf("page->frame_addr %d KB\n", temp->num * 4);
		temp = temp->next;
	}
}


/* Function creates virtual memory map */
vm_map_t *map_create(void)
{
	vm_map_t *map;
	
	if ((map = kmalloc(sizeof(vm_map_t))) == NULL)
		return NULL;
	
	if ((map->pmap = pmap_create()) == NULL)
		return NULL;
		
	map->segs = NULL;	
	return map;
}


/* Function creates virtual memory segment */
vm_seg_t *seg_create(page_t *pages, void *vaddr, uint_t flags)
{
	vm_seg_t *seg;
	
	if ((seg = kmalloc(sizeof(vm_seg_t))) == NULL)
		return NULL;
	
	seg->pages = pages;
	seg->vaddr = vaddr;
	seg->flags = flags;

	return seg;
}


/* Function maps segment to tasks virtual space */
int seg_map(vm_map_t *map, vm_seg_t *seg)
{
	uint_t k = 0;
	page_t *p;
	
	/* Add segment description to double linked list */
	if (map->segs == NULL) {
		map->segs = seg;
		seg->next = NULL;
		seg->prev = NULL;
	}
	else {
		seg->next = map->segs;
		map->segs->prev = seg;
		seg->prev = NULL;		
		map->segs = seg;
	}
	
	for (p = seg->pages; p != NULL; p = p->next)
		if (pmap_map(map->pmap, p, seg->vaddr + k++ * PAGE_SIZE, seg->flags) < 0)
			return -1;
	return 0;
}


/* Function releases task segments */
void release_segs(vm_map_t *map)
{
	vm_seg_t *lseg, *seg;
	
	lseg = NULL;	
	for (seg = map->segs; seg != NULL; seg = seg->next) {
		if (lseg != NULL)
			kfree(lseg);
		area_free(seg->pages);
		lseg = seg;
	}
	
	if (lseg)
		kfree(lseg);
	return;
}


/* Functions releases virtual memory map */
void map_free(vm_map_t *map)
{
	release_segs(map);
	pmap_free(map->pmap);
	kfree(map);
	return;
}
