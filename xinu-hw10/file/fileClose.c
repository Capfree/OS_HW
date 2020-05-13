/* fileClose.c - fileClose */
/* Copyright (C) 2007, Marquette University.  All rights reserved. */

#include <kernel.h>
#include <file.h>
#include <string.h>
#include <memory.h>

/*------------------------------------------------------------------------
 * fileClose - Close a file.
 *------------------------------------------------------------------------
 */
devcall fileClose(int fd)
{
    int result;
	struct dirblock *tmpDirlst;
	int i;
	bool isExtendTab = FALSE;
	
    if ((NULL == supertab) || (NULL == filetab) 
            || (FILE_FREE == filetab[fd].fn_state) || fd < 0 )
           // || fd >= supertab->sb_dirlst->db_fnodes[DIRENTRIES - 1].fn_length)

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
		isExtendTab = TRUE;
	}
	
    if (FILE_DIRTY & filetab[fd].fn_state)
    {
        seek(DISK0, filetab[fd].fn_blocknum);
        write(DISK0, filetab[fd].fn_data, DISKBLOCKLEN);
        filetab[fd].fn_state &= ~FILE_DIRTY;
    }
	
    filetab[fd].fn_state &= ~FILE_OPEN;
	
    wait(supertab->sb_dirlock);
	
	if(isExtendTab){
		tmpDirlst = malloc(sizeof(struct dirblock));		
		if (NULL == tmpDirlst){
			return SYSERR;
		}
		tmpDirlst->db_blocknum = filetab[fd - fd%DIRENTRIES].fn_blocknum;
		
		for(i = 0; i < DIRENTRIES; ++i){
			*(tmpDirlst->db_fnodes + i) = *(filetab + fd - fd%DIRENTRIES + i);
		}
		
		seek(DISK0, tmpDirlst->db_blocknum);
		result = write(DISK0, tmpDirlst, sizeof(struct dirblock));
	
		signal(supertab->sb_dirlock);

		if (SYSERR == result){
			return SYSERR;
		}		
	}else{
		seek(DISK0, supertab->sb_dirlst->db_blocknum);
		result = write(DISK0, supertab->sb_dirlst, sizeof(struct dirblock));
		signal(supertab->sb_dirlock);

		if (SYSERR == result){
			return SYSERR;
		}
	}

    return OK;
}
