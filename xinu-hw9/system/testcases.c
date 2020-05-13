/**
 * @file testcases.c
 * @provides testcases
 *
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

/* Function prototypes */
uchar getc(void);
void putc(uchar);
void testPrintFreeList(void);
message recvnow(void);
message recv(void);

/* Tests of send() and receive()*/

void testRecv(void){
	pid_typ pid;
    uint cpuid;
    message msg;
	
    cpuid = getcpuid();
    pid = currpid[cpuid];


	msg = recv();
		
	printf("Process %d received a message: %d \r\n", pid, msg);
	
    return;
}

void testRecvnow(void){
	pid_typ pid;
    uint cpuid;
    message msg;
	
    cpuid = getcpuid();
    pid = currpid[cpuid];

	msg = recvnow();
		
	if(SYSERR == msg){
		printf("Process %d has not received a message\r\n", pid);
	}else{
		printf("Process %d received a message: %d \r\n", pid, msg);
	}
	
    return;
}

/* Test malloc w/o free for the purpose of garbage collection. */
void testMallocWOfree(int size){
    int* testArr;

	testArr = (int*)malloc(sizeof(int) * size);
	
	*testArr = 1;
	*(testArr + size - 1) = 1;
	
	
    return;
}

/* Test process to use for testing semaphores. */
void testMalloc(void){
	uint cpuid;
    int i;
    int* testArr;
	
	cpuid = getcpuid();
	
	kprintf("Core %d::: No memory allocated yet.\r\n", cpuid);
	
	testPrintFreeList();
	testArr = (int*)malloc(sizeof(int) * 10);
	kprintf("Core %d::: allocated 10 ints.\r\n", cpuid);
	testPrintFreeList();	

	i = free(testArr);
	if(i == NULL){
		kprintf("Core %d::: free memory failed.\r\n", cpuid);
	}else{
		kprintf("Core %d::: freed 10 ints.\r\n", cpuid);
	}
	
	testPrintFreeList();
	
    return;
}

void testCreate(void){
    pid_typ pid;
    uint cpuid;
    pcb * ppcb;

    cpuid = getcpuid();

    pid = currpid[cpuid];
    ppcb = &proctab[pid];

    kprintf("Process %d, stack base 0x%08X, stack length 0x%08X, stack pointer 0x%08X \r\n", pid, ppcb->stkbase, ppcb->stklen, ppcb->regs[PREG_SP]);
    return;
}

void testPrintFreeList(void){
	uint cpuid;	
	register memblk *curr;
	//irqmask im;
	uint i;
	//im = disable();
	cpuid = getcpuid();
	lock_acquire(freelist[cpuid].memlock);

	
	curr = freelist[cpuid].head;
	i = 0;
	kprintf("Core %d::: memory space begin at 0x%08X, total size 0x%08X, free space 0x%08X \r\n", cpuid, freelist[cpuid].base, freelist[cpuid].bound, freelist[cpuid].length);
	
	while(NULL != curr){
			kprintf("Core %d::: %dth free block, starts at 0x%08X, with length 0x%08X \r\n\r\n", cpuid, i++, (uint)curr, (uint)curr->length);
			curr = curr->next;
	}
	
    lock_release(freelist[cpuid].memlock);
    //restore(im);
    return;
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void){
    uchar c;
    enable();
	
	int pid;
	message msg;

    printf("===TEST BEGIN===\r\n");
    

    printf("Base of freelist[0] 0x%08X \r\n", (uint)freelist[0].base);
	printf("Freelist[0] before any processes except main begin.\r\n");
    testPrintFreeList();
	printf("\r\n");
    printf("Please input 0~8 \r\n");
    printf("Case 0: main process send() msg. New process recv().\r\n");
	printf("Case 1: main process sendnow() msg. New process recv().\r\n");
    printf("Case 2: main process send() msg. New process recvnow().\r\n");
	printf("Case 3: main process sendnow() msg. New process recvnow().\r\n");
	printf("Case 4: new process malloc but no free. Notice if memory being freed after new process killed.\r\n");	
	printf("Case 5: main process send() msg. New process on core 1 recv().\r\n");
	printf("Case 6: main process sendnow() msg. New process on core 1 recv().\r\n");
    printf("Case 7: main process send() msg. New process on core 1 recvnow().\r\n");
	printf("Case 8: main process sendnow() msg. New process on core 1 recvnow().\r\n");
	
	printf("\r\n");
	printf("\r\n");
	
    msg = 1023;
	c = getc();
    switch (c)
    {
		case '0':
			pid = create((void *)testRecv, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0);
			ready(pid, RESCHED_YES, 0);
			send(pid, msg);		
			break;
			
		case '1':
			pid = create((void *)testRecvnow, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0);
			ready(pid, RESCHED_NO, 0);
			send(pid, msg);
			break;
			
		case '2':
			pid = create((void *)testRecv, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0);
			ready(pid, RESCHED_YES, 0);
			sendnow(pid, msg);
			break;
			
		case '3':
			pid = create((void *)testRecvnow, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0);
			ready(pid, RESCHED_NO, 0);
			sendnow(pid, msg);
			break;	

		case '4':
			ready(create((void *)testMallocWOfree, INITSTK, PRIORITY_HIGH, "PRINTER-0", 1, 10), RESCHED_NO, 0);
			ready(create((void *)testMallocWOfree, INITSTK, PRIORITY_HIGH, "PRINTER-1", 1, 1024), RESCHED_NO, 0);
			ready(create((void *)testMallocWOfree, INITSTK, PRIORITY_HIGH, "PRINTER-2", 1, 1024*1024), RESCHED_NO, 0);
			ready(create((void *)testMallocWOfree, INITSTK, PRIORITY_HIGH, "PRINTER-3", 1, 512), RESCHED_YES, 0);
            break;
		
		case '5':
			pid = create((void *)testRecv, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0);
			ready(pid, RESCHED_YES, 1);
			send(pid, msg);		
			break;
			
		case '6':
			pid = create((void *)testRecvnow, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0);
			ready(pid, RESCHED_NO, 1);
			send(pid, msg);
			break;
			
		case '7':
			pid = create((void *)testRecv, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0);
			ready(pid, RESCHED_YES, 1);
			sendnow(pid, msg);
			break;
			
		case '8':
			pid = create((void *)testRecvnow, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0);
			ready(pid, RESCHED_NO, 1);
			sendnow(pid, msg);
			break;	
		default:
	    break;
    }

    while (numproc > 5)  // Four null procs plus this test proc.
    {
        resched();
    }

    printf("\r\n===TEST END===\r\n");
    printf("\r\n\r\nAll user processes have completed.\r\n\r\n");
 
	printf("\r\n"); 
	printf("Freelist[0] after any processes except main end.\r\n");
    testPrintFreeList();


    //testPrintFreeList();
    while (1);
    return;
}

