/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Fine-grained memory allocator
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

#include <hal/current/locore.h>
#include <hal/current/types.h>
#include <hal/current//defs.h>
#include <init/std.h>
#include <vm/vm.h>


/*
 * Bucket header - allocated memory area is divided into small blocks (buckets).
 * Every free bucket has its small header. Area is prepared by prepare_kmalloc_area()
 * funtions after page allocation.
 */
typedef struct bucket_header {
	struct bucket_header *next;
} bucket_header_t;


/*
 * Header of bucket area.
 */
typedef struct area_header {
	struct area_header  *next;         /* pointer to the nex area in chain */
	bucket_header_t     *first_free;   /* first free bucket */
	uint_t              nbuckets;      /* number of buckets in rea */
	uint_t              size;          /* area size in bytes */
	uint_t              idx;           /* sizes[] table index */
} area_header_t;


/*
 * sizes[] item - size descriptor
 */
typedef struct kmalloc_size_descriptor {
	area_header_t  *first_area;      /* first area on the list */
	uint_t         nbuckets;         /* number of all created buckets */
	uint_t         alloc_buckets;    /* allocated buckets in all areas */
	uint_t         bucket_size;      /* bucket size */
} kmalloc_size_descriptor_t;


/*
 * sizes[] table - main hash structure used in kmalloc allocation
 */
kmalloc_size_descriptor_t sizes[] = {
	{ NULL, 0, 0, 32   },
	{ NULL, 0, 0, 64   },
	{ NULL, 0, 0, 128  },
	{ NULL, 0, 0, 252  },
	{ NULL, 0, 0, 508  },
	{ NULL, 0, 0, 1020 },
	{ NULL, 0, 0, 2040 },
	{ NULL, 0, 0, 0    }};


uint_t kmalloc_npages = 0;

/* kmalloc() access mutex */
mutex_t kmalloc_mutex;


/*
 * Function prepares new kmalloc bucket area. It returns number of created
 * buckets. Function isn't synchronized by kmalloc mutex.
 */
void prepare_kmalloc_area(area_header_t *areah, uint_t size, uint_t bucket_size, uint_t idx)
{
	bucket_header_t *fbh;      /* first bucket header */
	uint_t k;
	uint_t real_size;
	uint_t nbuckets;
	
	/* Calculate area parameters */
	real_size = bucket_size + sizeof(bucket_header_t);
	fbh = (bucket_header_t *)((void *)areah + sizeof(area_header_t));
	nbuckets = (size - sizeof(area_header_t)) / real_size;	
	
	/* Create free bucket list */
	for (k = 0; k < nbuckets - 1; k++)
		((bucket_header_t *)((void *)fbh + k * real_size))->next = (bucket_header_t *)((void *)fbh + (k + 1) * real_size);
	((bucket_header_t *)((void *)fbh + k * real_size))->next = NULL;
	
	areah->next = NULL;
	areah->first_free = fbh;
	areah->nbuckets = nbuckets;
	areah->size = size;
	areah->idx = idx;	
	return;
}


/* Function initializes kmalloc subsystem */
int kmalloc_init(uint_t npages)
{
	area_header_t *areah;
	uint_t k = 0;
	
	unlock(&kmalloc_mutex);
	
	/* Prepeare areas for all sizes[] entries */
	for (;;) {		
		if ((areah = (area_header_t *)kernel_pages_alloc(npages)) == NULL)
			return -1;
		
		prepare_kmalloc_area(areah, npages * PAGE_SIZE, sizes[k].bucket_size, k);
		kmalloc_npages = npages;
		
		sizes[k].first_area = areah;
		sizes[k].nbuckets = areah->nbuckets;
		sizes[k].alloc_buckets = 0;
		k++;
		
		if (sizes[k].bucket_size == 0)
			break;
	}
	return 0;
}	


/* Function allocates bucket */
void *kmalloc(uint_t size)
{
	uint_t real_size;
	uint_t idx;
	area_header_t *areah, *last_areah = NULL;
	bucket_header_t *bh;
	
	lock(&kmalloc_mutex);
	
	real_size  = size + sizeof(bucket_header_t);

	/* Find sizes[] entry */
	idx = 0;
	for (;;) {
		if (size <= sizes[idx].bucket_size)
			break;
	
		/* If end of sizes is reached return error */
		if (sizes[idx++].bucket_size == 0) {
#ifdef _DEBUG_KMALLOC
			printf("kmalloc: block too large (%d bytes).\n", size);
#endif
			unlock(&kmalloc_mutex);
			return NULL;
		}
	}

	/* Obtain first bucket pointer */
	if ((areah = sizes[idx].first_area) == NULL) {
#ifdef _DEBUG_KMALLOC
		printf("kmalloc subsystem isn't initialized\n");
#endif
		unlock(&kmalloc_mutex);
		return NULL;
	}
	
	/* Find free bucket */
	for (;;) {
		
		/* If free area doesn't exist allocate new one */
		if (areah == NULL) {		
#ifdef _DEBUG_KMALLOC
			printf("kmalloc: before kernel_pages_alloc()\n");
#endif
			if ((areah = (area_header_t *)kernel_pages_alloc(kmalloc_npages)) == NULL) {
#ifdef _DEBUG_KMALLOC
				printf("kmalloc: out of memory\n");
#endif
				unlock(&kmalloc_mutex);
				return NULL;
			}
			
			prepare_kmalloc_area(areah, kmalloc_npages * PAGE_SIZE, sizes[idx].bucket_size, idx);
			last_areah->next = areah;
			sizes[idx].nbuckets += areah->nbuckets;
		}
		
		/* Find free bucket - if it doesn't exist go to the next area */
		if (areah->first_free == NULL) {
			if (areah->next == NULL)
				last_areah = areah;
			areah = areah->next;
		}
		
		/* Uff - bucket is found */
		else {
			bh = areah->first_free;
			areah->first_free = areah->first_free->next;
			bh->next = (bucket_header_t *)areah;
			sizes[idx].alloc_buckets++;
			break;
		}		
	}
	
	unlock(&kmalloc_mutex);
	return ((void *)bh + sizeof(bucket_header_t));
}


/* Function releases allocated bucket */
void kfree(void *p)
{
	bucket_header_t *bh;
	area_header_t *areah;
	
	lock(&kmalloc_mutex);	
	bh = (bucket_header_t *)(p - sizeof(bucket_header_t));
	
	/* Obtain area header pointer */
	areah = (area_header_t *)bh->next;
	
	if (areah == NULL) {
#ifdef _DEBUG
		printf("kfree: bad bucket\n");
#endif
		unlock(&kmalloc_mutex);
		return;
	}
	
	/* Now release bucket */
	bh->next = areah->first_free;
	areah->first_free = bh;
	
	/* And update statistics */
	sizes[areah->idx].alloc_buckets--;
	
	unlock(&kmalloc_mutex);
	return;
}
