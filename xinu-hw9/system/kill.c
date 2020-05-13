/**
 * @file kill.c
 * Provides: kill
 *
 * COSC 3250/ COEN 4820 Assignment 4
 */

/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

/* Assembly atomic operations subroutines */
extern int _atomic_decrement_pre(int *);

/*
 * kill  --  kill a process and remove it from the system
 */
syscall kill(int pid)
{	
	irqmask im;
    pcb *ppcb;                  /* points to process control block for pid */
	struct memblock *prev;
	struct memblock *curr;
	struct memblock *block;
	im = disable();
    uint cpuid = getcpuid();

    if (isbadpid(pid) || (0 == pid) || (1 == pid)
        || (2 == pid) || (3 == pid)
        || (cpuid != proctab[pid].core_affinity)
        || (PRFREE == (ppcb = &proctab[pid])->state))
    {
        return SYSERR;
    }

    ppcb = &proctab[pid];

    _atomic_decrement(&numproc);

    if (numproc <= 4)
    {
        // all processes are done running
//              kprintf("\r\n\r\nAll user process have completed.\r\n\r\n");
//              extern void halt(void);
//              halt();
    }
	
	//Free garbage
	prev = curr = ppcb->myblocks;
	while(curr != NULL){
		prev = curr;
		curr = curr->next;
		block = (struct memblock*)(prev->length);
		++block;
		free(block);
        //freemem(block, block->length);
		freemem(prev, sizeof(struct memblock));
	}
	
	//Free msgin lock
	lock_free(ppcb->msginlock);
    switch (ppcb->state)
    {
    case PRCURR:
        ppcb->state = PRFREE;   /* suicide */
	    freemem(ppcb->stkbase, ppcb->stklen);
        //free(ppcb->stkbase);
       // restore(im);
        resched();

    case PRREADY:
        //free(ppcb->stkbase);
        remove(pid);

    default:
        ppcb->state = PRFREE;
        //free(ppcb->stkbase);
    }

	freemem(ppcb->stkbase, ppcb->stklen);
	restore(im);
    return OK;
}
