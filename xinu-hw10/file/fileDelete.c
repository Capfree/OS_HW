/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/*                                                                 */

#include <kernel.h>
#include <memory.h>
#include <file.h>
#include <string.h>

/*------------------------------------------------------------------------
 * fileDelete - Delete a file.
 *------------------------------------------------------------------------
 */
devcall fileDelete(int fd)
{
    // TODO: Unlink this file from the master directory index,
    //  and return its space to the free disk block list.
    //  Use the superblock's locks to guarantee mutually exclusive
    //  access to the directory index.
	int i, counter;
	struct dirblock *tmpDirlst;
	bool isExtendTab = FALSE;
    if ((NULL == supertab) || (NULL == filetab) || fd < 0 )//|| (isbadfd(fd)))
        //|| fd >= supertab->sb_dirlst->db_fnodes[DIRENTRIES - 1].fn_length ) 
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
	
	wait(supertab->sb_dirlock);
	
	sbFreeBlock(supertab,filetab[fd].fn_blocknum);
	

	filetab[fd].fn_state = FILE_FREE;
	//supertab->sb_dirlst->db_fnodes[fd].fn_state = FILE_FREE;
	if(isExtendTab){
		tmpDirlst = malloc(sizeof(struct dirblock));
		if (NULL == tmpDirlst){
			return SYSERR;
		}
		
		tmpDirlst->db_blocknum = filetab[fd - (fd%DIRENTRIES)].fn_length;

		for (i = 0; i < DIRENTRIES; ++i){
			*(tmpDirlst->db_fnodes + i) = *(filetab + i + fd - (fd%DIRENTRIES));
		}

		seek(DISK0, tmpDirlst->db_blocknum);
    
		if (SYSERR == write(DISK0, tmpDirlst, sizeof(struct dirblock))){
			free(tmpDirlst);
			signal(supertab->sb_dirlock);
			return SYSERR;
		}
	
		signal(supertab->sb_dirlock);
		free(tmpDirlst);
	}else{//for file id < DIRENTRIES
		tmpDirlst = supertab->sb_dirlst;
		seek(DISK0, tmpDirlst->db_blocknum);
    
		if (SYSERR == write(DISK0, tmpDirlst, sizeof(struct dirblock))){
			signal(supertab->sb_dirlock);
			return SYSERR;
		}
		
		signal(supertab->sb_dirlock);	
	}
	
    return OK;
}
