/**
 * @file testcases.c
 * @provides testcases
 *
 * $Id: testcases.c 175 2008-01-30 01:18:27Z brylow $
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

devcall putc(int dev, char c)
{
    return 0;
}


/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;

    kprintf("===TEST BEGIN===\r\n");

    c = kgetc();
    switch (c)
    {
		/*
		TEST:: kgetc() and kputc()
		*/
    case '0' ... '9':
        kprintf("Got a number!\r\n");
        break;
    case 'a' ... 'z':
        kprintf("Got a lower letter!\r\n");
        break;
    case 'A' ... 'Z':
        kprintf("Got an upper letter!\r\n");
        break;
    case NULL:
        kprintf("Got NULL!\r\n");
        break;
    default:
        kprintf("Got a sign other than numbers and letters!\r\n");
    }
	
	/*
	TEST:: kcheckc() and kungetc()
    */
	int k;

    k = kcheckc();
    if(k == NULL){//Initial unget buffer should be empty
        kprintf("kcheckc() test 0 passed!\r\n");
    }else{
        kprintf("kcheckc() test 0 failed!\r\n");        
    }

    k = kungetc(c);
    if(k == c){//Put c into unget buffer and return c
        kprintf("kcheckc() test 1 passed!\r\n");
    }else{
        kprintf("kcheckc() test 1 failed!\r\n");        
    }

    k = kgetc();
    if(k == c){//Get the oldest letter in unget buffer, c
        kprintf("kcheckc() test 3 passed!\r\n");
    }else{
        kprintf("kcheckc() test 3 failed!\r\n");        
    }

	/* 
	TEST:: kungetc() unget buffer size
	*/
	while(kcheckc() && kgetc()){//Free unget buffer
		
	}
	
	int i;
	for(i = 0; i <= 10; ++i){// UNGETMAX == 10
		k = kungetc(c);		
    }
	
	if(k == FALSE){//Exceed the buffer should return FALSE
		kprintf("Unget buffer size test passed!\r\n");
	}else{
		kprintf("Unget buffer size test failed!\r\n");
	}

	while(kcheckc() && kgetc()){//Free unget buffer
		
	}
    kprintf("\r\n===TEST END===\r\n");
    return;
}
