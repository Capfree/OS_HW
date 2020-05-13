/**
 * @file send.c
 * @provides IPC send
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup IPC
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  
 */
 
 /**
 Function send() takes a PID and a message, and sends it to the specified process. 
 Function send() should perform error checking on the process ID, and check whether the destination process already has a message. 
 If the destination process already has a message, 
	send() should instead place the outgoing message in its own PCB's msgout field, 
	enqueue itself on the destination process's msgqueue,
	and set its own state to PRSEND before yielding the processor.
 **/
 
 syscall send(int pid, message msg){
	pcb *destPcb;
	pcb *localPcb;
    irqmask im;

	if(isbadpid(pid))return SYSERR;
	
    im = disable();
	destPcb = &proctab[pid];
	localPcb = &proctab[currpid[getcpuid()]];
	
	lock_acquire(destPcb->msginlock);
	if(destPcb->hasMessage){
		
		
		/* set proc status */		
		localPcb->msgout = msg;
		localPcb->state = PRSEND;
		lock_release(destPcb->msginlock);
		enqueue(currpid[getcpuid()], destPcb->msgqueue);		
		resched();
	}else{
		destPcb->hasMessage = TRUE;
		destPcb->msgin = msg;
		lock_release(destPcb->msginlock);
		if(destPcb->state == PRRECV)
			ready(pid, TRUE, destPcb->core_affinity);
	}
	restore(im);	
	return OK;
	
}

 /**
 Function sendnow() takes a PID and a message, and sends it to the specified process. 
 Function sendnow() should perform error checking on the process ID, 
 and should return a SYSERR if the intended destination already has a message waiting. 
 If the intended destination process is already waiting for a message, sendnow() should unblock that process and yield the processor. 
 Function sendnow() should not block.
 */
 
syscall sendnow(int pid, message msg){
	pcb *destPcb;
    irqmask im;

	if(isbadpid(pid))return SYSERR;
	
    im = disable();

	destPcb = &proctab[pid];
	lock_acquire(destPcb->msginlock);
	if(destPcb->hasMessage){
		lock_release(destPcb->msginlock);
        restore(im);
		return SYSERR;
	}else if(destPcb->state == PRRECV){
		destPcb->hasMessage = TRUE;
		destPcb->msgin = msg;
		lock_release(destPcb->msginlock);
		ready(pid, TRUE, destPcb->core_affinity);		
        restore(im);
		return OK;
	}else{
		destPcb->hasMessage = TRUE;
		destPcb->msgin = msg;
		lock_release(destPcb->msginlock);
        restore(im);
		return OK;
	}	
}

