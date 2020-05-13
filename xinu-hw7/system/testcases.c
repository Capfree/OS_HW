/**
 * @file testcases.c
 * @provides testcases
 *
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

/* Function prototypes */
void print_sem(semaphore sem);
uchar getc(void);
void putc(uchar);


/* Test process to use for testing semaphores. */
process testSemWait(semaphore sem)
{
    wait(sem);
    printf("process %d after wait()\r\n", currpid[getcpuid()]);
    return 0;
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    uchar c;
    semaphore testsem;

    enable();

    kprintf("===TEST BEGIN===\r\n");
    
	// TODO: Test your operating system!
    
    kprintf("Please input p/s/m\r\n");
    kprintf("P for normal printf(), several numbers and letters should show up.\r\n");
    kprintf("S for core 0 printf the testsem info in 4 processes.\r\n");
    kprintf("M for 4 cores printf the testSem info. A chaos output expected. \r\n");
    c = getc();
    switch (c)
    {
		case 'p':
		case 'P':
			printf("\r\n0123456789\r\n");
			printf("\r\nabcdefghij\r\n");
			printf("\r\nABCDEFGHIJ\r\n");
            break;

		case 'm':
        case 'M':
            testsem = semcreate(4);
            ready(create((void *)testSemWait, INITSTK, PRIORITY_HIGH, "PRINTER-0", 1, testsem), RESCHED_YES, 0);
			ready(create((void *)testSemWait, INITSTK, PRIORITY_HIGH, "PRINTER-1", 1, testsem), RESCHED_YES, 1);
			ready(create((void *)testSemWait, INITSTK, PRIORITY_HIGH, "PRINTER-2", 1, testsem), RESCHED_YES, 2);
			ready(create((void *)testSemWait, INITSTK, PRIORITY_HIGH, "PRINTER-3", 1, testsem), RESCHED_YES, 3);
            break;

        case 's':
        case 'S':
            testsem = semcreate(4);
            ready(create((void *)testSemWait, INITSTK, PRIORITY_HIGH, "PRINTER-0", 1, testsem), RESCHED_NO, 0);
			ready(create((void *)testSemWait, INITSTK, PRIORITY_HIGH, "PRINTER-1", 1, testsem), RESCHED_NO, 0);
			ready(create((void *)testSemWait, INITSTK, PRIORITY_HIGH, "PRINTER-2", 1, testsem), RESCHED_NO, 0);
			ready(create((void *)testSemWait, INITSTK, PRIORITY_HIGH, "PRINTER-3", 1, testsem), RESCHED_YES, 0);
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

    while (1);
    return;
}

void print_sem(semaphore sem)
{
    struct sement *semptr;
    semptr = &semtab[sem];

    printf("\r\n%d->state: %s\r\n", sem,
           (semptr->state == 0x01) ? "SFREE" : "SUSED");
    printf("%d->count: %d\r\n", sem, semptr->count);
    printf("%d->queue: %s\r\n\r\n", sem,
           (isempty(semptr->queue)) ? "EMPTY" : "NONEMPTY");
}
