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


//*ASSUME: newfs is a virtual disk (a Linux file) containing an EXT2 file system.
//*
//*  RunningPtr
//*     |                                                  ||*********************
//*     |                                                  ||
//*     V        |---- PointerToCWD ------|                ||
//*              |                        |                || 
//*   PROC[ ]    |                        V  MINODE[ ]     ||        root dev
//* ===========  |                         ============    ||   ==================
//* |nextProcPtr |                         |  INODE   |    ||   |     INODEs   
//* |pid, ppid   |                         | -------  |    ||   ================== 
//* |uid         |                         | dev,ino  |    || 
//* |cwd --------|                         | refCount |    ||*********************
//* |                                      | dirty    |
//* |fd[10]                                | mounted  |         
//* | ------                               | mTablePtr| 
//* | ------                               |==========|<--<-        
//* | ------                               |  INODE   |    |      
//* | ------                               | -------  |    | 
//* ===========                            | dev,ino  |    |
//*                                        | refCount |    |
//*                                        |  dirty   |    |
//*                                        |  mounted |    |
//*                                     <--| mTablePtr|    |
//*                                     |  |==========|    |         
//*            PointAtRootInode         |                  |
//*                   ^                 |                  |
//*                   |                 V                  |
//*                   |  MountTable[ ]  |                  |       mount DEV
//*                   |  ------- 0 --------- 1 ---------   |     --------------
//*                   |  | devNumber    |   DEV      |     |     |  #2_INODE   |
//*                   |--| MinodePtr    |  MinodePtr-|----->     --------------
//*                      -------------------------------
//*                      | deviceName   |    name    |
//*                      ------------------------------- 


int mount(char *inFileSys, char *mount_point)
{
    if(inFileSys == 0 || mount_point == 0)
    {
        printf("\n\n-=0={Current Disk: \n\n", running->cwd->mptr);
        return -1;
    }

}


int umount(char *filesys)
{

}