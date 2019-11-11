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

extern char line[256], command[32], pathname[256];
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


    running->fd[0]->mode = mode;
    running->fd[0]->mptr = mip;
    running->fd[0]->refCount++;
    
    if(mode == RD || mode == WR || mode == RW)
    {
        running->fd[0]->offset = 0;    
    }
    else
    {
        running->fd[0]->offset = mip->INODE.i_size;
    }
    

    return;
}
