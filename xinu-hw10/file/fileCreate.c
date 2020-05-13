/* fileCreate.c - fileCreate */
/* Copyright (C) 2007, Marquette University.  All rights reserved. */

#include <kernel.h>
#include <file.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * fileCreate - Create a file.
 *------------------------------------------------------------------------
 */
devcall fileCreate(char *name)
{
    int fd = 0;
	int newDirlstBlockNum;
	struct dirblock *newDirlst;
	int counter, maxFilenum, i;
	struct filenode * tmpFiletab;
	
    if ((NULL == supertab) || (NULL == filetab))
    {
        return SYSERR;
    }

    if(0 == strncmp("DIRBLOCK", name, FNAMLEN)){
        printf("ERROR: file name can not be 'DIRBLOCK' \r\n");
        return SYSERR;
    }
	
			
	wait(supertab->sb_dirlock);
	
	while ((fd < DIRENTRIES) && (FILE_FREE != filetab[fd].fn_state)){
        ++fd;
    }
	
    if (fd >= DIRENTRIES){//Search in extend filetab

		//maxFilenum = supertab->sb_dirlst->db_fnodes[DIRENTRIES - 1].fn_length;
		if(0 == strncmp("DIRBLOCK", filetab[DIRENTRIES].fn_name, FNAMLEN)){//Find if extend filetab exist
			maxFilenum = filetab[DIRENTRIES].fn_length;
		}else{//Extend filetab not exist, create one
			tmpFiletab = malloc(sizeof(struct filenode)*(2*DIRENTRIES));
			if (NULL == tmpFiletab){
				return SYSERR;
			}
			for(i = 0; i < DIRENTRIES; ++i){
				*(tmpFiletab + i) = *(filetab + i);
			}
			
			newDirlstBlockNum =  sbGetBlock(supertab);
			if (SYSERR == newDirlstBlockNum){
				signal(supertab->sb_dirlock);
				free(tmpFiletab);
				return SYSERR;
			}
			
			newDirlst = malloc(sizeof(struct dirblock));
			if (NULL == newDirlst){
				free(tmpFiletab);
				return SYSERR;
			}
			
			newDirlst->db_blocknum = newDirlstBlockNum;
			for (counter = 0; counter < DIRENTRIES; ++counter){
				newDirlst->db_fnodes[counter].fn_state = FILE_FREE;
			}
			
			//keep old dirlst at first spot of new dirlst fnodes[]
			newDirlst->db_fnodes[0].fn_state = FILE_USED | FILE_OPEN;
			newDirlst->db_fnodes[0].fn_length = 2*DIRENTRIES; //Total number of files
			newDirlst->db_fnodes[0].fn_cursor = 0;//number of files in current block
			newDirlst->db_fnodes[0].fn_blocknum = newDirlstBlockNum;//Current dir block	
			//newDirlst->db_fnodes[DIRENTRIES - 1].fn_data = (char *)filetab;
			newDirlst->db_fnodes[0].fn_data = NULL;
			strcpy(newDirlst->db_fnodes[0].fn_name, "DIRBLOCK");
			newDirlst->db_fnodes[0].fn_name[FNAMLEN] = '\0';
			
			seek(DISK0, newDirlstBlockNum);
			if (SYSERR == write(DISK0, newDirlst, sizeof(struct dirblock))){
				signal(supertab->sb_dirlock);
				free(newDirlst);
				free(tmpFiletab);
				return SYSERR;
			}
			
			for(i = 0; i < DIRENTRIES; ++i){
				*(tmpFiletab + DIRENTRIES + i) = *(newDirlst->db_fnodes + i);
			}
			
			free(filetab);
			filetab = tmpFiletab;
			free(newDirlst);
			maxFilenum = filetab[DIRENTRIES].fn_length;
		}
		
		while ((fd < maxFilenum) && (FILE_FREE != filetab[fd].fn_state)){
			++fd;
		}
		
    }else{//Total number of files <= DIRENTRIES
		maxFilenum = DIRENTRIES;
	}
	
    //printf("1");
    
	//printf("2");
	if (fd >= maxFilenum){//create a new dir
		//tmpFiletab = malloc();
		tmpFiletab = malloc(sizeof(struct filenode)*(maxFilenum + DIRENTRIES));
		if (NULL == tmpFiletab){
			return SYSERR;
		}
		//memcpy(filetab, dirblk->db_fnodes, sizeof(struct filenode)*DIRENTRIES);
		//filetab = dirblk->db_fnodes;
		//printf("0000000000000000");
		
		//Record file # at filetab[DIRENTRIES] for future use
		filetab[DIRENTRIES].fn_length += DIRENTRIES;
		
		for(i = 0; i < maxFilenum; ++i){
			*(tmpFiletab + i) = *(filetab + i);
		}
		//printf("1111111111111111");
		newDirlstBlockNum =  sbGetBlock(supertab);
		if (SYSERR == newDirlstBlockNum){
			signal(supertab->sb_dirlock);
			filetab[DIRENTRIES].fn_length -= DIRENTRIES;
			return SYSERR;
		}
		//printf("New dirlist block %d\r\n", newDirlstBlockNum);
		//printf("22222222222222222");
		//newDirlst = (struct dirblock *)(SUPERBLOCKNUM + newDirlstBlockNum);
		newDirlst = malloc(sizeof(struct dirblock));
		if (NULL == newDirlst){
			filetab[DIRENTRIES].fn_length -= DIRENTRIES;
			return SYSERR;
		}
		newDirlst->db_blocknum = newDirlstBlockNum;
		for (counter = 0; counter < DIRENTRIES; ++counter){
			newDirlst->db_fnodes[counter].fn_state = FILE_FREE;
		}
		//printf("333333333333333333");
		//keep old dirlst at first spot of new dirlst fnodes[]
		newDirlst->db_fnodes[0].fn_state = FILE_USED | FILE_OPEN;
	    newDirlst->db_fnodes[0].fn_length = maxFilenum + DIRENTRIES; //Total number of files
        newDirlst->db_fnodes[0].fn_cursor = 0;//number of files in current block
        newDirlst->db_fnodes[0].fn_blocknum = newDirlstBlockNum;//Current block number	
        //newDirlst->db_fnodes[DIRENTRIES - 1].fn_data = (char *)filetab;
        newDirlst->db_fnodes[0].fn_data = NULL;
        strcpy(newDirlst->db_fnodes[0].fn_name, "DIRBLOCK");
        newDirlst->db_fnodes[0].fn_name[FNAMLEN] = '\0';

		//supertab->sb_dirlst = (struct dirblock *)newDirlstBlockNum;		
		
		seek(DISK0, newDirlstBlockNum);
		
		if (SYSERR == write(DISK0, newDirlst, sizeof(struct dirblock))){
			signal(supertab->sb_dirlock);
			filetab[DIRENTRIES].fn_length -= DIRENTRIES;
			free(newDirlst);
            return SYSERR;
        }
		//printf("444444444444444444444");
        //supertab->sb_dirlst = newDirlst;
		
		for(i = 0; i < DIRENTRIES; ++i){
			*(tmpFiletab + maxFilenum + i) = *(newDirlst->db_fnodes + i);
		}
		//printf("55555555555555555");
		free(filetab);
		filetab = tmpFiletab;
		free(newDirlst);
        ++fd;//DIRENTRIES saved for dirlist
    }

    //printf("3\r\n");
    filetab[fd].fn_length = 0;
    filetab[fd].fn_cursor = 0;
    strncpy(filetab[fd].fn_name, name, FNAMLEN);
    filetab[fd].fn_name[FNAMLEN] = '\0';
    filetab[fd].fn_data = (char *)malloc(DISKBLOCKLEN);
    if (NULL == filetab[fd].fn_data)
    {
        filetab[fd].fn_state = FILE_FREE;
        signal(supertab->sb_dirlock);
        return SYSERR;
    }
    filetab[fd].fn_state = FILE_USED | FILE_OPEN;

    signal(supertab->sb_dirlock);
    filetab[fd].fn_blocknum = sbGetBlock(supertab);
    if (SYSERR == filetab[fd].fn_blocknum){
        return SYSERR;
    }
    //printf("%d\r\n",fd);
    return fd;
}
