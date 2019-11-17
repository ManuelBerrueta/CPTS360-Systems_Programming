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

int map(INODE ino, int logicBlk, char buff[])
{
    int blk = -1;
    int ibuff[256] = { 0 };

    if(logicBlk < 12)
    {
        blk = ino.i_block[logicBlk];
    }
    else if(12 <= logicBlk < 12+256) //*indirect blocks
    {
        get_block(dev, ino.i_block[12], buff); //read i_block 12 into ibuff
        blk = ibuff[logicBlk - 12];
    }
    else
    {
        // TODO:  double indirect
    }
    return blk;
}


int my_read(int fd, char buf[], int numBytes)
{
    /* Preparations: 
    ASSUME: file is opened for RD or RW;
    ask for a fd  and  nbytes to read;
    verify that fd is indeed opened for RD or RW;
    return(myread(fd, buf, nbytes)); */
    int count = 0;  //* # of bytes to read
    char kbuff[BLKSIZE];
        
    printf("fdIndex = %d\n", fd);
    int numOfBytes = atoi(dirname2);

    if(!running->fd[fd])
    {
        puts("Read requires fd to be open!");
        return -1;
    }
    if(running->fd[fd]->mode != (RW || RD))
    {
        puts("Read requires fd to be open for RW or RD");
        return -1;
    }
    if(dirname2 == 0 || dirname2 == "") // numOfBytes
    {
        puts("Read requires # of bytes to read as a second argument!");
        return -1;
    }

    int offset = running->fd[fd]->offset;   //* Current file offest in file
    //* Compute bytes available in file
    int availableBytes = running->fd[fd]->mptr->INODE.i_size - offset;

    while(numBytes && availableBytes)
    {
        //*Compute Logical Block
        int logicalBlock = offset / BLKSIZE;
        //*Compute Start byte in Logical block
        int startByte = offset % BLKSIZE;

        //*Convert Logibal Block # to Physical Block # through INODE_i_block[ ]
        //int physicalBlock = running->fd[fd]->mptr->INODE.i_block[logicalBlock];
        int physicalBlock = map(running->fd[fd]->mptr->INODE, logicalBlock, buf);

        get_block(dev, physicalBlock, kbuff);

        char *cp = kbuff + startByte;

        int remaining = BLKSIZE - startByte;

        //TODO: (5) 
        while(remaining)
        {
            *buf++ = *cp++; //the contents of cp into buff 1 byte @ a time
            offset++;
            count++;
            remaining--;
            availableBytes--;
            numBytes--;
            if(numBytes == 0 || availableBytes == 0)
            {
                break;
            }
        }
    }
    return count;
}

//!NOTE: We use logical blocks because a file may be bigger then one block 

int cat(int fileName)
{
    char mybuff[1024], dummy = 0; //a null char at end of mybuff[]
    int n;

    int fd = open_file(fileName, RD);

    while(n = read(fd, mybuff, 1024))
    {
        mybuff[n] = 0;
        printf("%s", mybuff);
        //TODO: HANDLE '\n'
    }
    close(fd);
}