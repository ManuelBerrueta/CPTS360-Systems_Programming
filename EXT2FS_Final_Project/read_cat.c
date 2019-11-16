#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <sys/stat.h>
#include <ext2fs/ext2_fs.h>

#include "type.h"

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR; // need this for new version of e2fs

#define BLKSIZE 1024
extern GD *gp;
extern SUPER *sp;
extern INODE *ip;
extern DIR *dp;

extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC proc[NPROC], *running;
extern OFT fileTable[NOFT];

/********** globals *************/
extern int fd, dev;                         // dev = fd
extern int ninodes, nblocks;                // ninodes, nblocks numbers from SUPER
extern int bmap, imap, iblock, inode_start; // BMAP, IMAP, inodes start block numbers

extern char gpath[256]; // token strings
extern int n;           // number of token strings
extern char *name[64];  // pointers to token strings

extern char line[256], command[32], pathname[256], dirname2[256];
extern char dname[64]; //? Directory string holder
extern char bname[64]; //? Basename string holder
extern int bno;

char lbuff[1024];

//* int myread(int fd, char buf[ ], nbytes) behaves EXACTLY the same as the
//* read() system call in Unix/Linux. 
//* The algorithm of myread() can be best explained in terms of the following 
//* diagram.
//* 
//* (1).  PROC              (2).                          | 
//*      =======   |--> OFT oft[ ]                        |
//*      | pid |   |   ============                       |
//*      | cwd |   |   |mode=flags|                       | 
//*      | . ..|   |   |minodePtr ------->  minode[ ]     |      BlockDevice
//*      | fd[]|   |   |refCount=1|       =============   |   ==================
//*  fd: | .------>|   |offset    |       |  INODE    |   |   | INODE -> blocks|
//*      |     |       |===|======|       |-----------|   |   ==================
//*      =======           |              |  dev,ino  |   |
//*                        |              =============   |
//*                        |
//*                        |<------- avil ------->|
//*     -------------------|-----------------------
//*     |    |    | ...  |lbk  |   |  ...| .......|
//*     -------------------|---|------------------|-
//* lbk   0    1 .....     |rem|                   |
//*                      start                   fsize  


int myread(int fd, char buf[], int numBytes)
{
    
}

int read_file()
{
    /* Preparations: 
    ASSUME: file is opened for RD or RW;
    ask for a fd  and  nbytes to read;
    verify that fd is indeed opened for RD or RW;
    return(myread(fd, buf, nbytes)); */
    
    int fdIndex = atoi(pathname);
    printf("fdIndex = %d\n", fdIndex);

    int numOfBytes = atoi(dirname2);

    if(!running->fd[fdIndex])
    {
        puts("Read requires fd to be open!");
        return -1;
    }
    if(running->fd[fdIndex]->mode != (RW || RD))
    {
        puts("Read requires fd to be open for RW or RD");
        return -1;
    }
    if(dirname2 == 0 || dirname2 == "") // numOfBytes
    {
        puts("Read requires # of bytes to read as a second argument!");
        return -1;
    }
    return(myread(fd, lbuff, numOfBytes));
}