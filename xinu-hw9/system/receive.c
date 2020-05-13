/**
 * @file receive.c
 * @provides IPC receive
 *
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
Function recv() returns the message sent to the current process. 
If there is no message already, 
	recv() should block, 
	that is, put the current process into the PRRECV state and reschedule. 
If recv() sees other processes waiting to send in its msgqueue, 
	it should dequeue the process, 
	grab the message from that process's msgout field, 
	and unblock that process before returning.
**/

message recv(void){
	message msg;
	pid_typ pid;
	pcb *localPcb;
	pcb *sentPcb;
    irqmask im;
	bool waitflag = FALSE;

    im = disable();

	localPcb = &proctab[currpid[getcpuid()]];
	
	lock_acquire(localPcb->msginlock);
	if(!localPcb->hasMessage){	
		localPcb->state = PRRECV;
		lock_release(localPcb->msginlock);
		waitflag = TRUE;
		resched();
	}
	
	if(waitflag){
		lock_acquire(localPcb->msginlock);
	}
	
	msg = localPcb->msgin;
	localPcb->hasMessage = FALSE;
	if(nonempty(localPcb->msgqueue)){
		localPcb->hasMessage = TRUE;
		pid = dequeue(localPcb->msgqueue);
		sentPcb = &proctab[pid];
		localPcb->msgin = sentPcb->msgout;
		ready(pid, FALSE, sentPcb->core_affinity);
	}
	
	lock_release(localPcb->msginlock);
    restore(im);
	return msg;
}

/**
Function recvnow() returns the message sent to the current process. 
If there is no message already, 
	recv() should not block, 
	that is, it does not wait, and instead immediately returns a SYSERR.
**/


message recvnow(void){
	message msg;
	pid_typ pid;
	pcb *localPcb;
	pcb *sentPcb;
    irqmask im;

    im = disable();

	localPcb = &proctab[currpid[getcpuid()]];
	
	lock_acquire(localPcb->msginlock);
	if(!localPcb->hasMessage){
		if(nonempty(localPcb->msgqueue)){
			pid = dequeue(localPcb->msgqueue);
			sentPcb = &proctab[pid];
			msg = sentPcb->msgout;
			lock_release(localPcb->msginlock);
			ready(pid, FALSE, sentPcb->core_affinity);
            restore(im);
			return msg;
		}else{
			lock_release(localPcb->msginlock);
            restore(im);
			return SYSERR;
		}
	}
	
	msg = localPcb->msgin;
	localPcb->hasMessage = FALSE;
	if(nonempty(localPcb->msgqueue)){
		localPcb->hasMessage = TRUE;
		pid = dequeue(localPcb->msgqueue);
		sentPcb = &proctab[pid];
		localPcb->msgin = sentPcb->msgout;
		ready(pid, FALSE, sentPcb->core_affinity);
	}
	lock_release(localPcb->msginlock);
    restore(im);
	return msg;
}
