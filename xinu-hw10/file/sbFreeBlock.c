/* sbFreeBlock.c - sbFreeBlock */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/*                                                                 */

#include <kernel.h>
#include <device.h>
#include <memory.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 * sbFreeBlock - Add a block back into the free list of disk blocks.
 *------------------------------------------------------------------------
 */
devcall sbFreeBlock(struct superblock *psuper, int block){
    // TODO: Add the block back into the filesystem's list of
    //  free blocks.  Use the superblock's locks to guarantee
    //  mutually exclusive access to the free list, and write
    //  the changed free list segment(s) back to disk.
	
   if((block <= 1) || (block > DISKBLOCKTOTAL)){// Not able to free Superblock & dirlst
    	return SYSERR;
   }
   
	struct freeblock *nextFreeblk;
	struct dirblock *dirlst;
    struct dentry *phw;
    int diskfd;
	int counter;
	
    if (NULL == psuper){
        return SYSERR;
    }
	
    phw = psuper->sb_disk;
	
    if (NULL == phw){
        return SYSERR;
    }
    diskfd = phw - devtab;
	
    // Updating the free list must be mutually exclusive.
    wait(psuper->sb_freelock);
    
    struct freeblock *freeblk = psuper->sb_freelst;
    
    if (freeblk == NULL){//CASE 0: no free list exists
    	//printf("Freeblk is NULL\n");
    	freeblk = psuper->sb_freelst = malloc(sizeof(struct freeblock));
    	freeblk->fr_blocknum = block;
        freeblk->fr_count = 0;
        //freeblk->fr_next = NULL;
		freeblk->fr_next = 0;
        seek(diskfd, block);
		
        if (SYSERR == write(diskfd, freeblk, sizeof(struct freeblock))){
			signal(psuper->sb_freelock);
 	   		return SYSERR;
    	}
		 
		freeblk->fr_next = NULL;
		
    	nextFreeblk = freeblk;
    	dirlst = psuper->sb_dirlst;
		
    	psuper->sb_freelst = (struct freeblock *)block;
    	psuper->sb_dirlst = (struct dirblock *)(psuper->sb_dirlst->db_blocknum);
    	seek(diskfd, psuper->sb_blocknum);
    	if (SYSERR == write(diskfd, psuper, sizeof(struct superblock))){
			signal(psuper->sb_freelock);
 	   		return SYSERR;
    	}
		
    	psuper->sb_freelst = nextFreeblk;
    	psuper->sb_dirlst = dirlst;
    	signal(psuper->sb_freelock);
    	return OK;
    }
	
	//Seek the last freeblk
    while(freeblk->fr_next != NULL){
    	if (freeblk->fr_blocknum == block){//a freeblk holds input block
			signal(psuper->sb_freelock);
			return SYSERR;
		}
		    	
    	for (counter = 0; counter < freeblk->fr_count; ++counter){
			if (freeblk->fr_free[counter] == block){//block exists in freelist
				signal(psuper->sb_freelock);
				return SYSERR;
			}
    	}
    	freeblk = freeblk->fr_next;
    }
	
	
    if (freeblk->fr_blocknum == block){//The last freeblk holds block
			signal(psuper->sb_freelock);
			return SYSERR;
	}
	
    for (counter = 0; counter < freeblk->fr_count; ++counter){
		if (freeblk->fr_free[counter]== block){//block exists in freelist
			signal(psuper->sb_freelock);
			return SYSERR;
		}
    }
	
    
    if (freeblk->fr_count >= FREEBLOCKMAX){//CASE 1: the last freeblk is full
	   	nextFreeblk = freeblk->fr_next = malloc(sizeof(struct freeblock));
    	nextFreeblk->fr_blocknum = block;
    	nextFreeblk->fr_count = 0;
    	nextFreeblk->fr_next = 0;
   		
    	seek(diskfd, nextFreeblk->fr_blocknum);
    	if (SYSERR == write(diskfd, nextFreeblk, sizeof(struct freeblock))){
			signal(psuper->sb_freelock);
 	   		return SYSERR;
    	}
		nextFreeblk->fr_next = NULL;
    }else{//CASE 2: the last freeblk is not full
		freeblk->fr_free[freeblk->fr_count] = block;
   		freeblk->fr_count++;
   	}
    
	//Write the last freeblk to harddisk
	nextFreeblk = freeblk->fr_next;
	if (freeblk->fr_next != NULL){
		freeblk->fr_next = (struct freeblock *)(freeblk->fr_next->fr_blocknum);
	}else{
		freeblk->fr_next = 0;
	}
    seek(diskfd, freeblk->fr_blocknum);
	
    if (SYSERR == write(diskfd, freeblk, sizeof(struct freeblock))){
		signal(psuper->sb_freelock);
		return SYSERR;
    }
	
    freeblk->fr_next = nextFreeblk;
    
    signal(psuper->sb_freelock);
    return OK;
}
