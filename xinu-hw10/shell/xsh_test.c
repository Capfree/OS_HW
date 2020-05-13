/**
 * @file     xsh_test.c
 * @provides xsh_test
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * Shell command (test) is testing hook.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_test(int nargs, char *args[])
{
    //TODO: Test your O/S.
    //printf("This is where you should put some testing code. %d \n", nargs);
	int allocTable[254];
	int i, j, fd;
	int block;
	
    if (nargs > 1){
    	if (0 == strcmp("exhaust",args[1])){
	    	for (i = 0; i < 254; i++){
		    	allocTable[i] = sbGetBlock(supertab);
		    }
			
		    xsh_diskstat(0,NULL);
			
		    for (i = 0; i < 254; i++){
		    	sbFreeBlock(supertab,allocTable[i]);
		    }	
			
			xsh_diskstat(0,NULL);
			
		}else if (0 == strcmp("blkGetFree",args[1])){
	    	xsh_diskstat(0,NULL);	
	    	block = sbGetBlock(supertab);
			
	    	xsh_diskstat(0,NULL);
	    	sbFreeBlock(supertab,block);
			
	    	xsh_diskstat(0,NULL);
	    }else if (0 == strcmp("cycle",args[1])){
	    	for (i = 0; i < 1000; i++){
	    		block = sbGetBlock(supertab);
	    		xsh_diskstat(0,NULL);
				
	    		sbFreeBlock(supertab,block);
	    		xsh_diskstat(0,NULL);
	    	}
	    } else if (0 == strcmp("freed",args[1])){
	    	block = sbGetBlock(supertab);
	    	xsh_diskstat(0,NULL);
    		sbFreeBlock(supertab,block);
    		xsh_diskstat(0,NULL);
    		int error = sbFreeBlock(supertab,block);
    		printf("error is %d\n",error);
    	} else if (0 == strcmp("files",args[1])){
			char fileName[FNAMLEN + 1];
			strcpy(fileName, "TESTFL0");
			fileName[FNAMLEN] = '\0';
			
			for(i = 0; i < FILEMAX; ++i){
				fd = fileCreate(fileName);
				if (SYSERR == fd){
					printf("ERROR creating file \"%s\"\n", fileName);
				}else{
					for(j = 0; j < FNAMLEN; ++j ){
						filePutChar(fd, fileName[j]);
					}
				}
				fileName[FNAMLEN-1] = fileName[FNAMLEN-1] + 1;
			}
	    	
	    	xsh_diskstat(0,NULL);
    		
			strcpy(fileName, "TESTFL0");
			fileName[FNAMLEN] = '\0';
			
			for(i = 0; i < FILEMAX; ++i){
				fd = fileOpen(fileName);
				if (SYSERR == fd){
					printf("File \"%s\" not found.\n", fileName);				
				}else{
					fileDelete(fd);
				}
				
				fileName[FNAMLEN-1] = fileName[FNAMLEN-1] + 1;
			}
			
    		xsh_diskstat(0,NULL);
    		
    	} else{
		   	printf("Usage: test <command> \n\t exhaust \n\t blkGetFree \n\t cycle \n\t freed \n\t files\n");
		}
    } else{
    	printf("Usage: test <command> \n\t exhaust \n\t blkGetFree \n\t cycle \n\t freed \n\t files \n");
	}
	    
    return OK;
}
