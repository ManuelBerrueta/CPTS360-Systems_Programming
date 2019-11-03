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

extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC proc[NPROC], *running;

#define BLKSIZE 1024
extern GD *gp;
extern SUPER *sp;
extern INODE *ip;
extern DIR *dp;

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

int link()
{
    int oldino = getino(pathname); //Get the inode number for the path
    MINODE *oldmip = iget(dev, oldino);
    MINODE *newmip;
    int newino;

    if(S_ISDIR(oldmip->INODE.i_mode)) 
    {
        printf("-={0  parentmip MUST NOT  be a DIR  0}=-\n");
        printf("-={0  LINK FAILED  0}=-\n") ;
        iput(oldmip);
        return;
    }

    newino = getino(dirname2);
}

int unlink()
{

}

int symlink()
{
    //TODO: Create new file and change to link type to file type 
}

int readlink()
{

}