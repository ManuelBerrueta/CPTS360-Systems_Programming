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

//* The algorithm of write_file() can also be explained in terms of the following
//* figure.
//* 
//* (1).  PROC              (2).                          | 
//*      =======   |--> OFT oft[ ]                        |
//*      | pid |   |   ============                       |
//*      | cwd |   |   |mode=flags|                       | 
//*      | . ..|   |   |minodePtr ------->  minode[ ]     |      BlockDevice
//*      | fd[]|   |   |refCount=1|       =============   |   ==================
//*  fd: | .------>|   |offset    |       |  INODE    |   |   | INODE -> blocks|
//*      |     |       |===|======|       |-----------|   |   ==================
//*      =======           |              | dev,inode |   | 
//*                        |              |  dirty    |   |
//*                        |              =============   |
//*                        |
//*     -------------------|-----------------
//*     |    |    | ...  |lbk  | ............
//*     -------------------|-----------------
//* lbk   0    1 .....     |rem|            |
//*                      start           fileSize (in INODE)  

int my_write(int fd, char *buf, int nbytes) {
    int lbk, start, blk, remain, i, diskBlk;
    OFT *oftp = running->fd[fd];
    MINODE *mip = oftp->mptr;
    int indirect_blk[256], dub_indirect_blk[256];
    char writebuf[BLKSIZE];
    int count;
    char *cq = buf;

    if(running->fd[fd] == 0)
    {
        printf("-=0={ERROR fd='%d' is NOT open, file must be open prior to writing!\n\n", fd);
        return -1;
    }

    while (nbytes > 0)
    {
        lbk = oftp->offset / BLKSIZE;
        start = oftp->offset % BLKSIZE;
    
        if (lbk < 12)
        {
            if (mip->INODE.i_block[lbk] == 0) {
                mip->INODE.i_block[lbk] = balloc(mip->dev); 
            }
            blk = mip->INODE.i_block[lbk];
            diskBlk = blk;
        }
        else if (lbk >= 12 && lbk < 256 + 12)
        {
            if (mip->INODE.i_block[12] == 0) {
                mip->INODE.i_block[12] = balloc(mip->dev);
                get_block(mip->dev, mip->INODE.i_block[12], indirect_blk);
                for (i = 0; i < 256; i++) {
                    indirect_blk[i] = 0;
                }
                put_block(mip->dev, mip->INODE.i_block[12], indirect_blk);
            }
            get_block(mip->dev, mip->INODE.i_block[12], indirect_blk);
            blk = indirect_blk[lbk - 12];
            if (blk == 0)
            {
                blk = balloc(mip->dev);
                indirect_blk[lbk - 12] = blk;
                put_block(mip->dev, mip->INODE.i_block[12], indirect_blk);
            }
            diskBlk = blk;
        }
        else {
            if (mip->INODE.i_block[13] == 0) {    //alloc i_block[13]
                mip->INODE.i_block[13] = balloc(mip->dev);
                get_block(mip->dev, mip->INODE.i_block[13], indirect_blk);
                for (i = 0; i < 256; i++) {
                    indirect_blk[i] = 0;
                }
                put_block(mip->dev, mip->INODE.i_block[13], indirect_blk);
            }
            blk = (lbk - 256 - 12) / BLKSIZE;            
            if (indirect_blk[blk] == 0) {
                indirect_blk[blk] = balloc(mip->dev);
                get_block(mip->dev, indirect_blk[blk], dub_indirect_blk);
                for (i = 0; i < 256; i++) {
                    dub_indirect_blk[i] = 0;
                }
                put_block(mip->dev, indirect_blk[blk], indirect_blk);
            }
            blk = (lbk - 256 - 12) % BLKSIZE;
            if (dub_indirect_blk[blk] == 0) {
                dub_indirect_blk[blk] = balloc(mip->dev);
            }
            diskBlk = dub_indirect_blk[blk];
        }
        get_block(mip->dev, diskBlk, writebuf);

        char *cp = writebuf + start;

        remain = BLKSIZE - start; // remaining space in buf for writing

        while (remain > 0)
        {
            if(remain <= nbytes)
            {
                oftp->offset += remain;
                count += remain;
                mip->INODE.i_size += remain;
                //strncpy(buf, cp, remain);
                strncpy(cp, buf, remain);
                nbytes -= remain;
                remain =0;
            }
            else
            {
                oftp->offset += nbytes;
                count += nbytes;
                mip->INODE.i_size += nbytes;
                //strncpy(buf, cp, nbytes);
                strncpy(cp, buf, nbytes);
                nbytes = 0;
                if(nbytes == 0)
                {
                    break;
                }
            }
        }


/*         while (remain > 0)
        {
            *cp++ = *cq++;
            nbytes--;
            remain--;
            count++;
            oftp->offset++;
            if (oftp->offset > mip->INODE.i_size) {
                mip->INODE.i_size++;
            }
            if (nbytes <= 0) break;
        } */
        put_block(mip->dev, diskBlk, writebuf);
    }
    mip->dirty = 1;
    printf("wrote %d char into file descriptor fd=%d\n", count, fd);
    return count;
}

int cp(char source[], char dest[])
{
    int fd = open_file(source, 0);
    int gd = open_file(dest, 1);
    int n = 0;
    char cpybuff[BLKSIZE];

    if(fd < 0)
    {
        printf("-=0={ERROR: File: %s FAILED to open\n", source);
    }
    if(gd < 0)
    {
        printf("-=0={ERROR: File: %s FAILED to open\n", source);
        printf("-=0={CREAT File: %s \n", source);
        creat_file(dest);
        gd = open_file(dest, 1);
        if(gd < 0)
        {
            printf("-=0={ERROR: CREAT File: %s \n", source);
        }
    }

    int debug = 0;
    while(n = my_read(fd, cpybuff, 1024))
    {
        if (debug == 11) 
            debug *= -1;

        my_write(gd, cpybuff, n);
        memset(cpybuff, 0, BLKSIZE);

        debug++;
    }
    close(fd);
    close(fd);
    return 0;
}

//TODO: mv 
int mv(char source[], char dest[])
{
    MINODE *smip, *dmip;
    char tempbuff[256];
    int ino;

    strcpy(tempbuff, dest);
    dbname(tempbuff);

    ino = getino(dname);
    dmip = iget(dev, ino);

    ino = getino(source);
    smip = iget(dev, ino);

    if(smip->dev == dmip->dev)
    {
        link();
    }
    else
    {
        cp(source, dest);
    }
    unlink();
}