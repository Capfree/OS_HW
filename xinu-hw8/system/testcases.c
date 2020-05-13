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

/* Test process to use for testing semaphores. */
void testMalloc(void)
{
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

void testPrintFreeList(void)
{
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
void testcases(void)
{
    uchar c;
    enable();

    int i;
	int *testArr;
    printf("===TEST BEGIN===\r\n");
    
	// TODO: Test your operating system!
    
    //TEST 1: malloc & free;




    //TEST 2: gigantic malloc & free
   // testArr = malloc(sizeof(int) * 1024 * 1024 * 100);
    //i = free(testArr);

    //if(SYSERR == i){
       //printf("Free memory failed. \r\n");
   // }
/*
    int* ptr;
    ptr = (int *)malloc(10 * sizeof(int));
    if(NULL == ptr){

    }else{
        int i;
        for(i = 0; i < 10; ++i){
            *(ptr+i) = i;
        }

        for(i = 0; i < 10; ++i){
            printf("%d\r\n",*(ptr+i));
        }
    }
*/
    printf("Base of freelist[0] 0x%08X \r\n", (uint)freelist[0].base);
	printf("Freelist[0] before any processes except main begin.\r\n");
    testPrintFreeList();
	printf("\r\n");
    printf("Please input 0~8 \r\n");
    printf("Case 0: allocate memory for 10 ints.\r\n");
	printf("Case 1: use up all memory of core 1 & free.\r\n");
	printf("Case 2: allocate memory for 1G bytes.\r\n");
	printf("Case 3: allocate and free memory for 10 ints.\r\n");
	printf("Case 4: free memory for NULL.\r\n");
	printf("Case 5: core 1 allocate memory for 10 ints.\r\n");
	printf("Case 6: core 0 creates 4 processes allocate and free memory for 10 ints.\r\n");
	printf("Case 7: each core allocate and free memory for 10 ints.\r\n");
	printf("Case 8: create 4 processes & print their pcb info.\r\n\r\n\r\n");
	
	
    c = getc();
    switch (c)
    {
		case '0':
			printf("No memory allocated yet.\r\n");
			testPrintFreeList();
			testArr = (int*)malloc(sizeof(int) * 10);
			printf("Tried allocate 10 ints.\r\n");
			testPrintFreeList();
			break;
			
		case '1':
			printf("No memory allocated yet.\r\n");
			testPrintFreeList();
			testArr = (int*)malloc(freelist[0].head->length - sizeof(memblk));
			kprintf("Tried use up all memory.\r\n");
			testPrintFreeList();
			
			i = free(testArr);
			if(i == NULL){
				printf("Free memory failed.\r\n");
			}else{
				printf("Freed all malloced memory.\r\n");
			}
			testPrintFreeList();
			break;
			
		case '2':
			printf("No memory allocated yet.\r\n");
			testPrintFreeList();
			testArr = (int*)malloc(1024 * 1024 * 1024);
			kprintf("Tried allocate 1G bytes.\r\n");
			testPrintFreeList();
			break;
			
		case '3':
			printf("No memory allocated yet.\r\n");
			testPrintFreeList();
			testArr = (int*)malloc(sizeof(int) * 10);
			kprintf("Allocated 10 ints.\r\n");
			testPrintFreeList();
			i = free(testArr);
			if(i == NULL){
				printf("Free memory failed.\r\n");
			}else{
				printf("Freed 10 ints.\r\n");
			}
			testPrintFreeList();
			break;	

		case '4':
			i = free(NULL);
			if(i == SYSERR){
				printf("Free memory failed.\r\n");
			}else{
				printf("Freed 10 ints.\r\n");
			}
            break;
			
		case '5':
			ready(create((void *)testMalloc, INITSTK, PRIORITY_HIGH, "PRINTER-1", 0), RESCHED_YES, 1);
            break;			

        case '6':
            ready(create((void *)testMalloc, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0), RESCHED_NO, 0);
			ready(create((void *)testMalloc, INITSTK, PRIORITY_HIGH, "PRINTER-1", 0), RESCHED_NO, 0);
			ready(create((void *)testMalloc, INITSTK, PRIORITY_HIGH, "PRINTER-2", 0), RESCHED_NO, 0);
			ready(create((void *)testMalloc, INITSTK, PRIORITY_HIGH, "PRINTER-3", 0), RESCHED_YES, 0);
            break;
			
        case '7':
            ready(create((void *)testMalloc, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0), RESCHED_YES, 0);
			ready(create((void *)testMalloc, INITSTK, PRIORITY_HIGH, "PRINTER-1", 0), RESCHED_YES, 1);
			ready(create((void *)testMalloc, INITSTK, PRIORITY_HIGH, "PRINTER-2", 0), RESCHED_YES, 2);
			ready(create((void *)testMalloc, INITSTK, PRIORITY_HIGH, "PRINTER-3", 0), RESCHED_YES, 3);
            break;

		case '8':
            ready(create((void *)testCreate, INITSTK, PRIORITY_HIGH, "PRINTER-0", 0), RESCHED_NO, 0);
			ready(create((void *)testCreate, INITSTK, PRIORITY_HIGH, "PRINTER-1", 0), RESCHED_NO, 0);
			ready(create((void *)testCreate, INITSTK, PRIORITY_HIGH, "PRINTER-2", 0), RESCHED_NO, 0);
			ready(create((void *)testCreate, INITSTK, PRIORITY_HIGH, "PRINTER-3", 0), RESCHED_YES, 0);
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

