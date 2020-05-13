/**
 * @file     main.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>

void testcases(void);

/**
 * Main thread.  You can modify this routine to customize what Embedded Xinu
 * does when it starts up.  The default is designed to do something reasonable
 * on all platforms based on the devices and features configured.
 */
thread main(void)
{
    kprintf("Hello Xinu World!\r\n");

    kprintf("Size of int :: %d \r\n", sizeof(int));
    kprintf("Size of char :: %d \r\n", sizeof(int));
    kprintf("Size of uint :: %d \r\n", sizeof(unsigned int));
    kprintf("Size of long :: %d \r\n", sizeof(long));
    kprintf("Size of ulong :: %d \r\n", sizeof(unsigned long));
    kprintf("Size of long long :: %d \r\n", sizeof(long long));
    //testcases();

    while (1){
        //testcases();
    }

    return 0;
}
