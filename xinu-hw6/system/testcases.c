/**
 * @file testcases.c
 * @provides testcases
 *
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>
extern int _atomic_increment_post(int *);
int preemStatus[3] = {0, 0, 0};

void printpid(int times)
{
	int i = 0;
	uint cpuid = getcpuid();

	enable();
	for (i = 0; i < times; i++)
	{
		kprintf("This is process %d\r\n", currpid[cpuid]);
		udelay(3);
	}
}

void preemExec(int times, int localPriority){
    int i = 0;
    enable();

    for(i = 0; i < times; i++){
        //kprintf("%d\r\n", localPriority);
        //preemStatus[localPriority] = preemStatus[localPriority] + 1;
        _atomic_increment_post(&(preemStatus[localPriority]));

        if(localPriority == PRIORITY_LOW){
            kprintf("%d :: %d :: %d \r\n", preemStatus[PRIORITY_LOW], preemStatus[PRIORITY_MED], preemStatus[PRIORITY_HIGH]);
        }
        udelay(QUANTUM);
    }
    
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	uchar c;

	kprintf("===TEST BEGIN===\r\n");
	kprintf("0) Test priority scheduling\r\n");
	kprintf("\'A\') Aging / Starvation testcase\r\n");
	kprintf("\'P\') Preemption testcase\r\n");

	// TODO: Test your operating system!

	c = kgetc();
	switch (c)
	{
		case '0':
			// Run 3 processes with varying priorities
			ready(create((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-A", 1, 5), RESCHED_NO, 0);
			ready(create((void *)printpid, INITSTK, PRIORITY_MED , "PRINTER-B", 1, 5), RESCHED_NO, 0);
			ready(create((void *)printpid, INITSTK, PRIORITY_LOW , "PRINTER-C", 1, 5), RESCHED_YES, 0);
				
			break;

		case 'a':
		case 'A':
#if AGING
			// AGING TESTCASE
			kprintf("AGING is enabled.\r\n");
			ready(create((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-A", 1, 30), RESCHED_NO, 0);
			ready(create((void *)printpid, INITSTK, PRIORITY_MED , "PRINTER-B", 1, 10), RESCHED_NO, 0);
			ready(create((void *)printpid, INITSTK, PRIORITY_LOW , "PRINTER-C", 1, 5), RESCHED_YES, 0);
			// TODO: Create a testcase that demonstrates aging 


#else
			// STARVING TESTCASE
			kprintf("\r\nAGING is not currently enabled.\r\n");
			
			// TODO: Create a testcase that demonstrates starvation
			ready(create((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-A", 1, 10), RESCHED_NO, 0);
			ready(create((void *)printpid, INITSTK, PRIORITY_MED , "PRINTER-B", 1, 10), RESCHED_NO, 0);
			ready(create((void *)printpid, INITSTK, PRIORITY_LOW , "PRINTER-C", 1, 10), RESCHED_YES, 0);

#endif
			break;

		case 'p':
		case 'P':
#if PREEMPT
			// PREEMPTION TESTCASE00
			kprintf("\r\nPreemption is enabled.\r\n");

            //int pid;

            //pid = create((void *)preemExec, INITSTK, PRIORITY_HIGH, "PRINTER-A", 2, 10, PRIORITY_HIGH);

            ready(create((void *)preemExec, INITSTK, PRIORITY_HIGH, "PRINTER-A", 2, 30, PRIORITY_HIGH), RESCHED_NO, 0);
            //kprintf("%d :: %d \r\n", proctab[pid].regs[0], proctab[pid].regs[1]);
            //ready(pid, RESCHED_NO, 0);
			ready(create((void *)preemExec, INITSTK, PRIORITY_MED , "PRINTER-B", 2, 10, PRIORITY_MED), RESCHED_NO, 0);
			ready(create((void *)preemExec, INITSTK, PRIORITY_LOW , "PRINTER-C", 2, 10, PRIORITY_LOW), RESCHED_YES, 0);

			// TODO: Create a testcase that demonstrates preemption


#else
			kprintf("\r\nPreemption is not currently enabled...\r\n");
            ready(create((void *)preemExec, INITSTK, PRIORITY_HIGH, "PRINTER-A", 2, 30, PRIORITY_HIGH), RESCHED_NO, 0);
            //kprintf("%d :: %d \r\n", proctab[pid].regs[0], proctab[pid].regs[1]);
            //ready(pid, RESCHED_NO, 0);
			ready(create((void *)preemExec, INITSTK, PRIORITY_MED , "PRINTER-B", 2, 10, PRIORITY_MED), RESCHED_NO, 0);
			ready(create((void *)preemExec, INITSTK, PRIORITY_LOW , "PRINTER-C", 2, 10, PRIORITY_LOW), RESCHED_YES, 0);


#endif
			break;
		
		default:
			break;
	}

	kprintf("\r\n===TEST END===\r\n");
	return;
}
