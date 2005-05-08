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

#include <hal/current/defs.h>
#include <hal/current/types.h>
#include <hal/current/pmap.h>

#include <init/std.h>
#include <vm/vm.h>
#include <vm/kmalloc.h>


/* Top of statically allocated kernel physical memory */
uint_t kernel_mem_top = KERNEL_BSS_END;


/*
 * Number of page dir elements used by kernel. It's calculated
 * during pmap_init() functions.
 */ 
uint_t needed_entries;


/* Function initializes page directory */
static inline void pmap_initpdir(uint_t *pdir)
{
	memclr(pdir, PAGE_DIR_SIZE * sizeof(pdentry_t));
	return;
}


/* Function initializes page table - does linear mapping starting from addr */
static inline void pmap_initptable(ptentry_t *ptable, uint_t addr, uint_t flags)
{
	uint_t k;
		
	for (k = 0; k < PAGE_TABLE_SIZE; k++)
		*(ptentry_t *)(ptable + k) = ((addr + k * 0x1000) & 0xfffff000) | flags;
	return;
}


/*
 * Function maps page table given by ptable at minimal adress given by minaddr.
 * When needed page directory entry is allocated, page table will be mapped at next
 * free address. Function returns address at which map has been performed.
 */
void *pmap_maptable(pdentry_t *pdir, ptentry_t *ptable, void *minaddr, uint_t flags)
{
	uint_t k;	
		
	*(pdir + 0) = ((*(pdir + 0)) & 0xfffffffe);
	
	for (k = (uint_t)GET_PDIR_IDX(minaddr); k < PAGE_DIR_SIZE; k++) {
		
		/* If entry is empty do mapping */
		if (*(pdir + k) == 0) {			
			*(pdir + k) = (((uint_t)KERNEL_TO_PHYS(ptable) & 0xfffff000) | flags);			
			return (void *)(k * PAGE_DIR_SIZE * PAGE_SIZE);
		}	
	}
	
	return NULL;
}		


/*
 * Function initializes machine dependent pmap interface. In initialization
 * procedure next kernel page tables are created (neccesary to map all physical memory
 * in kernel space). Top of the staticaly allocated kernel memory is increased.
 * num_of_ptable = 1 GB / 4 MB = 256
 * kernel_mem_top += size / PAGE_DIR_SIZE / PAGE_SIZE 
 */
void pmap_init(uint_t size)
{
	uint_t needed_size;
	ptentry_t *ptables, *addr;
	uint_t k;
	
	needed_size = size / PAGE_DIR_SIZE;
	
	/* Patch for computers witch small RAM < 4MB */
	if (needed_size > PAGE_TABLE_SIZE * sizeof(ptentry_t))
		needed_size -= PAGE_TABLE_SIZE * sizeof(ptentry_t);
	
	ptables = (ptentry_t *)PHYS_TO_KERNEL(kernel_mem_top);
	
	/* Static kernel memory allocation */
	kernel_mem_top += needed_size;
	needed_entries = needed_size / PAGE_SIZE;
	
	/* Initialize and map kernel page tables */
	for (k = 0; k < needed_entries; k++) {
		pmap_initptable(ptables + k * PAGE_TABLE_SIZE, (k + 1) * 0x00400000,
		                PGHD_PRESENT | PGHD_SYSTEM | PGHD_WRITE);		
		addr = pmap_maptable((pdentry_t *)PHYS_TO_KERNEL(KERNEL_PAGE_DIR), ptables + k * PAGE_TABLE_SIZE,
		                     (void *)KERNEL_BASE + (k + 1) * 0x400000,
		                     PTHD_PRESENT | PTHD_SYSTEM | PTHD_WRITE);
		                     
#ifdef _DEBUG_PMAP
		printf("mapped: [%x] %d MB\n", addr, (uint_t)addr / 1024 / 1024);	
#endif
	}
	__flush_tlb();	
}	


/* Function fills page_t structure for page frame given by pn */
page_t *pmap_getphyspage(uint_t size, page_t *page, uint_t pn)
{
	uint_t map_size = size / PAGE_SIZE * sizeof(page_t);
	uint_t paddr = pn * PAGE_SIZE;
	
	page->flags = 0;
	page->next = NULL;
	page->prev = NULL;
	page->num = pn;
	
	/* 0 - 4 KB - range - page isn't present for NULL pointer exception */
	if ((paddr >= 0) && (paddr <= PAGE_SIZE))
		page->flags = PG_RSVD | PG_KERNEL;
		
	/* 640KB - 1MB - ROM BIOS and device memory space */
	else if ((paddr >= 0xa0000) && (paddr <= 0x100000))
		page->flags = PG_RSVD;

	/*  kernel reserved physical memory */
	else if ((paddr >= KERNEL_CODE_START) && (paddr <= kernel_mem_top + map_size))
		page->flags = PG_RSVD | PG_KERNEL;
	
	else {
	
		/* physcial memory for DMA < 16 MB */
		if (paddr < 0x1000000) {
			page->flags = PG_DMA;
		}
		
		page->flags |= PG_PRESENT;
	}
	return page;
}


/* Function returns top of statically allocated kernel memory */
uint_t pmap_get_kmem_top(void)
{
	return kernel_mem_top;
}


/* Function creates empty pmap structure */
pmap_t *pmap_create(void)
{
	pmap_t *pmap;
	
	if ((pmap = (pmap_t *)kmalloc(sizeof(pmap_t))) == NULL)
		return NULL;
	pmap->npages = 0;
	
	if ((pmap->pdir = kernel_pages_alloc(1)) == NULL)
		return NULL;	
	memclr(pmap->pdir, PAGE_SIZE);
	
	/* Map kernel page dirs */
	memcpy(pmap->pdir + (KERNEL_BASE / PAGE_DIR_SIZE / PAGE_SIZE) * 4,
	       PHYS_TO_KERNEL(KERNEL_PAGE_DIR) + (KERNEL_BASE / PAGE_DIR_SIZE / PAGE_SIZE) * 4,
	       needed_entries * 4);

	return pmap;
}


/* Functions maps page at specified address */
int pmap_map(pmap_t *pmap, page_t *page, void *addr, uint_t flags)
{
	uint_t pd_idx;
	uint_t pt_idx;
	void *ptable;
	
	pd_idx = (uint_t)addr >> 22;
	pt_idx = (((uint_t)addr >> 12) & 0x000003ff);
	
	/* If no page table is allocated add new one */
	if (*((uint_t *)pmap->pdir + pd_idx) == 0) {
		if ((ptable = kernel_pages_alloc(1)) == NULL)
			return -1;
		*((uint_t *)pmap->pdir + pd_idx) = ((uint_t)KERNEL_TO_PHYS(ptable) & 0xfffff000)
		                                   | PTHD_PRESENT | PTHD_WRITE | PTHD_READ | PTHD_USER;
		memclr(ptable, PAGE_SIZE);
	}
	else {
		ptable = (void *)(*((uint_t *)pmap->pdir + pd_idx) & 0xfffff000);
		ptable = PHYS_TO_KERNEL(ptable);
	}
	
	/* Now map page */
	*((uint_t *)ptable + pt_idx) = ((page->num * PAGE_SIZE) & 0xfffff000) | flags;
	
	return 0;
}


/* Functions releases pmap structure */
void pmap_free(pmap_t *pmap)
{
	uint_t ptable;
	uint_t k;
	
	if (pmap == NULL) return;
	
	for (k = 0; k < PAGE_DIR_SIZE; k++) {
		ptable = (*(uint_t *)((uint_t *)pmap->pdir + k)) & 0xfffff000;
		
		/* Free page tables allocated by task. Don't free kernel page dirs and the NULL page. */
		if (ptable && (k < KERNEL_BASE / PAGE_DIR_SIZE / PAGE_SIZE)) {
#ifdef _DEBUG_PMAP
			printf("free entry: %d\n", k);
#endif
			kernel_pages_free(PHYS_TO_KERNEL(ptable));
		}
	}
	
	kernel_pages_free(pmap->pdir);
	kfree(pmap);
	return;
}
