/**
 * @file getmem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Allocate heap memory.
 *
 * @param nbytes
 *      Number of bytes requested.
 *
 * @return
 *      ::SYSERR if @p nbytes was 0 or there is no memory to satisfy the
 *      request; otherwise returns a pointer to the allocated memory region.
 *      The returned pointer is guaranteed to be 8-byte aligned.  Free the block
 *      with memfree() when done with it.
 */
void *getmem(ulong nbytes)
{
    register memblk *prev, *curr, *leftover;
    irqmask im;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (ulong)roundmb(nbytes);

    im = disable();

    /* TODO:
     *      - Use cpuid to use correct freelist
     *           ex: freelist[cpuid]
     *      - Acquire memory lock (memlock)
     *      - Traverse through the freelist
     *        to find a block that's suitable 
     *        (Use First Fit with remainder splitting)
     *      - Release memory lock
     *      - return memory address if successful
     */
    uint cpuid = getcpuid();
    lock_acquire(freelist[cpuid].memlock);
	
	if(nbytes > freelist[cpuid].length){
		lock_release(freelist[cpuid].memlock);
		restore(im);
		return (void *)SYSERR;
	}
	
    curr = freelist[cpuid].head;
    prev = NULL;

    while(NULL != curr){
        if(nbytes == curr->length){
            freelist[cpuid].length -= nbytes;

            if(NULL == prev){
                freelist[cpuid].head = curr->next;
                lock_release(freelist[cpuid].memlock);
                restore(im);
                return curr;
            }        
            
            prev->next = curr->next;
            lock_release(freelist[cpuid].memlock);
            restore(im);
            return curr;
        }

        if(nbytes < curr->length){
            freelist[cpuid].length -= nbytes;
            leftover = curr + nbytes/sizeof(memblk);
            leftover->length = curr->length - nbytes;
            leftover->next = curr->next;

            if(NULL == prev){
                freelist[cpuid].head = leftover;      
				//kprintf("CPUID: %d   0x%08X\r\n", cpuid, (uint)freelist[cpuid].head->next);
				//kprintf("CPUID: %d   curr 0x%08X, head 0x%08X, leftover 0x%08X\r\n", cpuid, curr, freelist[cpuid].head, leftover);
				//kprintf("CPUID: %d   curr 0x%08X, head 0x%08X, freespace 0x%08X\r\n", cpuid, curr, freelist[cpuid].head, freelist[cpuid].length);
                lock_release(freelist[cpuid].memlock);
                restore(im);
                return curr;
            }
            
            prev->next = leftover;
		    //kprintf("CPUID: %d   curr 0x%08X, head 0x%08X, freespace 0x%08X\r\n", cpuid, curr, freelist[cpuid].head, freelist[cpuid].length);
            lock_release(freelist[cpuid].memlock);
            restore(im);
            return curr;

        }
        
        prev = curr;
        curr = curr->next;
    }

    lock_release(freelist[cpuid].memlock);
    restore(im);
    return (void *)SYSERR;
}
