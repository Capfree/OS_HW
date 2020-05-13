/**
 * @file create.c
 * @provides create, newpid, userret
 *
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <arm.h>
#include <xinu.h>

/* Assembly routine for atomic operations */
extern int _atomic_increment_post(int *);
extern int _atomic_increment_limit(int *, int);

static pid_typ newpid(void);
void userret(void);
//void *getstk(ulong);

/**
 * Create a new process to start running a function.
 * @param funcaddr address of function that will begin in new process
 * @param ssize    stack size in bytes
 * @param name     name of the process, used for debugging
 * @param nargs    number of arguments that follow
 * @return the new process id
 */
syscall create(void *funcaddr, ulong ssize, ulong priority, char *name, ulong nargs, ...)
{
	ulong *saddr;               /* stack address                */
	//ulong *argddr;               /* argument address                */
	ulong pid;                  /* stores new process id        */
	pcb *ppcb;                  /* pointer to proc control blk  */
	ulong i;
	va_list ap;                 /* points to list of var args   */
	ulong pads = 0;             /* padding entries in record.   */
	void INITRET(void);

	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (ulong)(ssize + 3) & 0xFFFFFFFC;
	/* round up to even boundary    */
	//saddr = (ulong *)getstk(ssize);     /* allocate new stack and pid   */
    //saddr = (ulong *)malloc(ssize + sizeof(ulong));
	//saddr = (ulong *)getmem(ssize) + ssize - sizeof(ulong); 
    saddr = (ulong *)getmem(ssize);
    saddr += (ssize/sizeof(ulong));
    --saddr;
    //if(NULL == saddr){
      //  return SYSERR;
    //}

    //saddr = saddr + ssize/sizeof(ulong);
	pid = newpid();
	/* a little error checking      */
	if ((((ulong *)SYSERR) == saddr) || (SYSERR == pid))
	{
		return SYSERR;
	}
    
	_atomic_increment_post(&numproc);

	ppcb = &proctab[pid];
	/* setup PCB entry for new proc */
	ppcb->state = PRSUSP;

	// TODO: Setup PCB entry for new process.
	strncpy(ppcb->name, name, PNMLEN);
	ppcb->stkbase = saddr - ssize/sizeof(ulong) + 1;
	
    //kprintf("Saddr 0x%08X, stack base 0x%08X\r\n", saddr, ppcb->stkbase);
	ppcb->stklen  = ssize;
	//ppcb->core_affinity = pid%4;

    ppcb->sem = -1;
	ppcb->core_affinity = -1;
	ppcb->priority = priority;
	ppcb->hasMessage = FALSE;
	ppcb->msgin = 0;
	ppcb->msgout = 0;
	ppcb->msgqueue = newqueue();
	ppcb->msginlock = lock_create();
	ppcb->myblocks = NULL;
	/* Initialize stack with accounting block. */
	*saddr = STACKMAGIC;
	*--saddr = pid;
	*--saddr = ppcb->stklen;
	*--saddr = (ulong)ppcb->stkbase;

	/* Handle variable number of arguments passed to starting function   */
	if (nargs)
	{
		pads = ((nargs - 1) / 4) * 4;
	}
	/* If more than 4 args, pad record size to multiple of native memory */
	/*  transfer size.  Reserve space for extra args                     */
	for (i = 0; i < pads; i++)
	{
		*--saddr = 0;
	}
	
	//argddr = saddr;
	
	// TODO: Initialize process context.
	/*for (i=0;i < 64/sizeof(long);i++)
	{
		saddr--;
		*saddr = 0;
	}*/
	
    for (i = 0; i < PREGS; i++){
        ppcb->regs[i] = 0;
    }

    ppcb->regs[PREG_PC] = (int)funcaddr;
    ppcb->regs[PREG_LR] = (int)INITRET;
    ppcb->regs[PREG_SP] = (int)saddr;
	// TODO:  Place arguments into activation record.
	//        See K&R 7.3 for example using va_start, va_arg and
	//        va_end macros for variable argument functions.
	
	va_start(ap, nargs);
	
    for(i = 0; i < nargs; ++i){
        //intVal = va_arg(ap, int);
        //memcpy(saddr + i, &intVal, sizeof(int));
        if (i < 4)
            ppcb->regs[i] = va_arg(ap, ulong);
        else
            saddr[i-4]=va_arg(ap,ulong);
    }
	va_end(ap);
	//ppcb->regs[PREG_SP] = (long)saddr;

    /*
    for(i = 0; i < 4; ++i){
        kprintf("%d  ", ppcb->regs[i]);
    }
    kprintf("\r\n");
    */
	return pid;
}

/**
 * Obtain a new (free) process id.
 * @return a free process id, SYSERR if all ids are used
 */
static pid_typ newpid(void)
{
	pid_typ pid;                /* process id to return     */
	static pid_typ nextpid = 0;

	for (pid = 0; pid < NPROC; pid++)
	{                           /* check all NPROC slots    */
		//        nextpid = (nextpid + 1) % NPROC;
		_atomic_increment_limit(&nextpid, NPROC);
		if (PRFREE == proctab[nextpid].state)
		{
			return nextpid;
		}
	}
	return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
	uint cpuid = getcpuid();
	kill(currpid[cpuid]);
}
