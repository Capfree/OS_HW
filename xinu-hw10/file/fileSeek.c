/* fileSeek.c - fileSeek */
/* Copyright (C) 2007, Marquette University.  All rights reserved. */

#include <kernel.h>
#include <file.h>
#include <string.h>

/*------------------------------------------------------------------------
 * fileSeek - Seek to a new offset within a file.
 *------------------------------------------------------------------------
 */
devcall fileSeek(int fd, int offset)
{
    if ((NULL == supertab) || (NULL == filetab) 
            || (FILE_FREE == filetab[fd].fn_state) || fd < 0 )
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
	
	
    if ((filetab[fd].fn_cursor < 0) ||
        (filetab[fd].fn_cursor > filetab[fd].fn_length) ||
        (filetab[fd].fn_cursor > DISKBLOCKLEN))
    {
        return SYSERR;
    }

    if ((offset < 0) ||
        (offset > filetab[fd].fn_length) || (offset >= DISKBLOCKLEN))
    {
        return SYSERR;
    }

    filetab[fd].fn_cursor = offset;
    return OK;
}
