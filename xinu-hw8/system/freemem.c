/**
 * @file freemem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Frees a block of heap-allocated memory.
 *
 * @param memptr
 *      Pointer to memory block allocated with memget().
 *
 * @param nbytes
 *      Length of memory block, in bytes.  (Same value passed to memget().)
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail
 *      because of memory corruption or specifying an invalid memory block.
 */
syscall freemem(void *memptr, ulong nbytes)
{
    register struct memblock *block, *next, *prev;
    irqmask im;
    ulong top;

    /* make sure block is in heap */
    if ((0 == nbytes)
        || ((ulong)memptr < (ulong)memheap)
        || ((ulong)memptr > (ulong)platform.maxaddr)
		|| (memptr == NULL))
    {
        return SYSERR;
    }

    block = (struct memblock *)memptr;
    nbytes = (ulong)roundmb(nbytes);

    im = disable();

    /* TODO:
     *      - Determine correct freelist to return to
     *        based on block address
     *      - Acquire memory lock (memlock)
     *      - Find where the memory block should
     *        go back onto the freelist (based on address)
     *      - Find top of previous memblock
     *      - Make sure block is not overlapping on prev or next blocks
     *      - Coalesce with previous block if adjacent
     *      - Coalesce with next block if adjacent
     */
	
	//Determine which freelist memptr belongs to
    uint cpuid;
    for(cpuid = 0; cpuid < NPROC; ++cpuid){
        ulong base, tail;
        base =(ulong)(freelist[cpuid].base);
        tail = base + (ulong)(freelist[cpuid].bound);
        if((ulong)memptr >= base && (ulong)memptr < tail)
            break;
    }
	
	if(NPROC == cpuid)return SYSERR;
    
    lock_acquire(freelist[cpuid].memlock);
    prev = NULL;
    next = freelist[cpuid].head;
    top = (ulong)block + nbytes;

    while(next != NULL){
        if(top < (ulong)next){			
          //  kprintf("Free case 1 \r\n");
            //Before head
            if(NULL == prev){
				freelist[cpuid].length += nbytes;
				freelist[cpuid].head = block;
				freelist[cpuid].head->length = nbytes;
				freelist[cpuid].head->next = next;
                lock_release(freelist[cpuid].memlock);
                restore(im);
                return OK;               
            }

            //Block adjacent with prev
            if((ulong)block == (ulong)prev + prev->length){
                freelist[cpuid].length += nbytes;
                prev->length += nbytes;
                lock_release(freelist[cpuid].memlock);
                restore(im);
                return OK;               
            }

            //Block not adjacent with prev
            if((ulong)block > (ulong)prev + prev->length){
                freelist[cpuid].length += nbytes;
                prev->next = block;
                block->length = nbytes;
                block->next = next;
                lock_release(freelist[cpuid].memlock);
                restore(im);
                return OK;               
            } 
			
			//Free unallocated memory is forbidden.
			if((ulong)block < (ulong)prev + prev->length){
                lock_release(freelist[cpuid].memlock);
                restore(im);
                return SYSERR;               
            } 
        }

        //block adjacent with next
        if(top == (ulong)next){
        //    kprintf("Free case 2 \r\n");
             //Before head
            if(NULL == prev){
				freelist[cpuid].length += nbytes;
				freelist[cpuid].head = block;
				freelist[cpuid].head->length = nbytes + next->length;
				freelist[cpuid].head->next = next->next;
                lock_release(freelist[cpuid].memlock);
                restore(im);
                return OK;               
            }

            //Block adjacent with prev
            if((ulong)block == (ulong)prev+prev->length){
                freelist[cpuid].length += nbytes;
                prev->length += nbytes;
                prev->length += next->length;
                prev->next = next->next;
                lock_release(freelist[cpuid].memlock);
                restore(im);
                return OK;               
            }

            //Block not adjacent with prev
            if((ulong)block > (ulong)prev+prev->length){
                freelist[cpuid].length += nbytes;
                prev->next = block;
                block->length = nbytes + next->length;
                block->next = next->next;
                lock_release(freelist[cpuid].memlock);
                restore(im);
                return OK;               
            }
				
			//Free unallocated memory is forbidden.
			if((ulong)block < (ulong)prev + prev->length){
                lock_release(freelist[cpuid].memlock);
                restore(im);
                return SYSERR;               
            }
           
        }
        prev = next;
        next = next->next;
    }

    //next = NULL
    //Freelist is empty
    if(NULL == prev){		
		freelist[cpuid].head = block;
		freelist[cpuid].head->length = nbytes;
		freelist[cpuid].head->next = NULL;
		freelist[cpuid].length += nbytes;
        lock_release(freelist[cpuid].memlock);
        restore(im);
        return OK;               
    }

    //Block adjacent with prev
    if((ulong)block == (ulong)prev + prev->length){
        freelist[cpuid].length += nbytes;
        prev->length += nbytes;
        prev->next = next;
        lock_release(freelist[cpuid].memlock);
        restore(im);
        return OK;               
    }

    //Block not adjacent with prev
    if((ulong)block > (ulong)prev + prev->length){
        freelist[cpuid].length += nbytes;
        prev->next = block;
        block->length = nbytes;
        block->next = next;
        lock_release(freelist[cpuid].memlock);
        restore(im);
        return OK;               
    } 

    //Block not adjacent with prev
    if((ulong)block < (ulong)prev + prev->length){
        lock_release(freelist[cpuid].memlock);
        restore(im);
        return SYSERR;               
    } 
    lock_release(freelist[cpuid].memlock);
    restore(im);
    return OK;
}
