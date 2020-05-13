/**
 * @file free.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup libxc
 *
 * Attempt to free a block of memory based on malloc() accounting information
 * stored in preceding two words.
 *
 * @param ptr
 *      A pointer to the memory block to free.
 */
syscall free(void *ptr)
{
    struct memblock *block;
	struct memblock *prev;
	struct memblock *curr;
	pcb *localPcb;
    /* TODO:
     *      1) set block to point to memblock to be free'd (ptr)
     *      2) find accounting information of the memblock
     *      3) call freemem on the block with its length
     */
    if(NULL == ptr){
        return SYSERR;
    }
	
    block = (struct memblock*)ptr;
    --block;
	
	if (((void *)block < (void *)memheap) || ((void *)block > (void *)platform.maxaddr))
		return SYSERR;
	
	if (block->length == 0)
		return SYSERR;
	
	if ((void *)((ulong)block + block->length) > (void *)platform.maxaddr)
		return SYSERR;
	
	//To avoid free memory illegally
	if ((void *)block->next != (void *)&(block->next))
		return SYSERR;

    freemem(block, block->length);
			
	localPcb = &proctab[currpid[getcpuid()]];
	prev = curr = localPcb->myblocks;
	
	while(curr != NULL){
		if(curr->length == (ulong)block){
			prev->next = curr->next;
			freemem(curr, sizeof(struct memblock));
            break;
		}
		prev = curr;
		curr = curr->next;
	}

    return OK;
}
