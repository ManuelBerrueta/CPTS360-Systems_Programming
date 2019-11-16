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

/* Open File Data Structures:

    running
      |                                                  
      |                                                    ||****************** 
    PROC[ ]              OFT[ ]              MINODE[ ]     ||      Disk dev
  ===========    |---> ===========    |--> ============    || ===============
  |ProcPtr  |    |     |mode     |    |    |  INODE   |    || |      INODE   
  |pid, ppid|    |     |refCount |    |    | -------  |    || =============== 
  |uid      |    |     |minodePtr|---->    | dev,ino  |    || 
  |cwd      |    |     |offset   |         | refCount |    ||******************
  |         |    |     ====|======         | dirty    |
  |  fd[10] |    |         |               | mounted  |         
  | ------  |    |         |               ============
0 |   ----->|--->|         |
  | ------  |              |   
1 |         |              |
  | ------  |             --------------------------------
2 |         |             |0123456.............
  | ------  |             --------------------------------    
  ===========        logical view of file: a sequence of bytes */

int open_file(char *filePath, int mode)
{
    MINODE *parentmip;
    MINODE *mip;
    int parentino;
    char inPath[128] = {0};

    int ino = getino(filePath);
    if (ino == 0)
    {
        printf("-=0={ FILE %S, does NOT exist\n-=0={creat %s\n", filePath, filePath);
        strcpy(inPath, filePath);
        dbname(inPath);
        parentino = getino(dname);
        if(parentino == 0)
        {
            printf("-=0={ ERROR: getino failed on parentino: %s\n", dname);
            return 0;
        }

        parentmip = iget(dev, parentino);
        my_creat(parentmip, bname);

        ino = getino(filePath);
        if(ino == 0)
        {
            printf("-=0={ ERROR: getino failed on new file: %s\n", filePath);
            return 0;
        }
    }
    mip = iget(dev, ino);

    OFT *oftptr = allocateOFT();
    oftptr->mode = mode;
    oftptr->mptr = mip;
    oftptr->refCount++;

    if(mode == RD || mode == WR || mode == RW)
    {
        oftptr->offset = 0;    
    }
    else
    {
        oftptr->offset = mip->INODE.i_size;
    }
    
    int i=0;
    for(i=0; i < NFD; i++)
    {
        if(running->fd[i] == 0)
        {
            running->fd[i] = oftptr;
            break;
        }
    }

    switch (mode)
    {
    case 0:
        mip->INODE.i_atime = time(0L);
        mip->dirty = 1;
        break;
    case 1:
    case 2:
    case 3:
        mip->INODE.i_atime = mip->INODE.i_mtime = time(0L);
        mip->dirty = 1;
        break;
    }

    return i; //*Returning FD index
}


int close(int fd)
{
    OFT *oftptr;
    MINODE * mip;

    if(fd >= NFD) //Checking out of bounds
    {
        printf("-=0={ ERROR: fd out of range\n");
        return -1;
    }
    if(running->fd[fd] == 0)
    {
        printf("-=0={ERROR: FILE IS NOT OPEN!");
        return -1;
    }

    oftptr = running->fd[fd];
    running->fd[fd] = 0;
    oftptr->refCount--;

    if(oftptr->refCount == 0)
    {
        mip = oftptr->mptr;
        iput(mip);
    }
    return 0;
}


/* int lseek(int fd, int position)
{
    OFT *oftptr;
    int pos;

    oftptr = running->fd[fd];

    pos = oftptr->offset;
    
    if(position >= oftptr->mptr->INODE.i_size || position < 0)
    {
        printf("-=0={ERROR: Out Of Bounds\n");
        return -1;
    }

    oftptr->offset = position;

    return pos;
} */

int pfd()
{
    int i=0;
    for(i=0; i < NFD; i++)
    {
        printf("%d ", i);
        
        switch (running->fd[i]->mode)
        {
        case 0:
            printf("READ ");
            break;
        case 1:
            printf("WRITE ");
            break;
        case 2:
            printf("RD|WR ");
            break;
        case 3:
            printf("APPED ");
            break;
        }
        printf("%d ", running->fd[i]->offset);
        printf("[%d, %d]\n", running->fd[i]->mptr->dev, running->fd[i]->mptr->ino);
    }
}

int dup(int fd)
{
    int i;
    if(running->fd[fd] == 0)
    {
        printf("-=0={ERRROR: NOT AN OPEN FILE\n");
        return -1;
    }
    for(i=0; i < NFD; i++)
    {
        if(running->fd[i] == 0)
        {
            running->fd[i] = running->fd[fd];
            return i;
        }
    }
    printf("-=0={ERROR: CANNOT DUP fd\n");
    return -1;
}

int dup2(int fd, int gd)
{
    close(gd);
    dup(fd); 
}