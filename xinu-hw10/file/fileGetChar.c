/* fileGetChar.c - fileGetChar */
/* Copyright (C) 2007, Marquette University.  All rights reserved. */

#include <kernel.h>
#include <file.h>
#include <string.h>

/*------------------------------------------------------------------------
 * fileGetChar - Get a byte from a file.
 *------------------------------------------------------------------------
 */
devcall fileGetChar(int fd)
{
    char c;
    if ((NULL == supertab) || (NULL == filetab) 
            || (FILE_FREE == filetab[fd].fn_state) || fd < 0)
             //|| fd >= supertab->sb_dirlst->db_fnodes[DIRENTRIES - 1].fn_length)
        //|| (isbadfd(fd)) || (FILE_FREE == filetab[fd].fn_state))
    {
        return SYSERR;
    }
	
	if(fd >= DIRENTRIES){
		if(0 == strncmp("DIRBLOCK", filetab[DIRENTRIES].fn_name, FNAMLEN)){//Find if extend filetab exist
			if(fd >= filetab[DIRENTRIES].fn_length)
				return SYSERR;
		}else{
			return SYSERR;
		}
	}
	
	
    if(filetab[fd].fn_data == NULL){
        return SYSERR;
    }

    //if(fd < 0 || fd > )
    if ((filetab[fd].fn_cursor < 0) ||
        (filetab[fd].fn_cursor >= filetab[fd].fn_length))
    {
        return SYSERR;
    }

    c = filetab[fd].fn_data[filetab[fd].fn_cursor];
    if (filetab[fd].fn_cursor < filetab[fd].fn_length)
    {
        filetab[fd].fn_cursor++;
    }

    return c;
}
