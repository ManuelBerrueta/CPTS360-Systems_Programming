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
MOUNT mntTable[NMTABLE];


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
    int i=0;
    int mdev;
    MINODE *mip;
    int ino;
    MOUNT *mp;
    char sbuf[BLKSIZE] = {0};
    char gbuf[BLKSIZE] = {0};

    if(strcmp(inFileSys, "") == 0 && strcmp(mount_point, "") == 0)
    {
        printf("\n-=0={Current Disk: \n");

        for(int i=0; i < NMTABLE; i++){
            if( mntTable[i].dev != 0){
                printf("dev#=%d  |  name=%s  |  mount_point=%s\n", mntTable[i].dev, mntTable[i].mntName, mntTable[i].mntName);
            }
        }
    }
    else
    {
        for(i=0; i < NMTABLE; i++){
            if(strcmp(mntTable[i].devName, inFileSys) == 0){
                printf("-=0={ERROR: Filesystem=%s ALREADY MOUNTED!!\n", inFileSys);
                return -1;
            }
        }
        for(i =0; i < NMTABLE; i++){
            if(mntTable[i].dev == 0){
                mp = &mntTable[i];
                break;
            }
        }

        if(i == NMTABLE){
            printf("-=0={NO Mounting Space Available!\n");
            return -1;
        }

        mdev = open(inFileSys, O_RDWR);
        if(mdev <= 0){
            printf("-=0]{ERROR FAILED TO OPEN FILESYSTEM=%s for RDWR!\n", inFileSys);
            return -1;
        }

        get_block(mdev, SUPERBLOCK, sbuf);
        sp = (SUPER *)sbuf;

        if(sp->s_magic != MAGIC){
            printf("-=0=]{ERROR: Filesystem is NOT EXT2\n");
            return -1;
        }

        mp->dev = mdev;
        mp->free_inodes = sp->s_free_inodes_count;
        mp->free_blocks = sp->s_free_blocks_count;
        mp->nblocks = sp->s_blocks_count;
        mp->ninodes = sp->s_inodes_count;

        get_block(mdev, GDBLOCK, gbuf);

        gp = (GD *)gbuf;
        mp->imap = gp->bg_inode_bitmap;
        mp->bmap = gp->bg_block_bitmap;
        mp->iblock = gp->bg_inode_table;

        strcpy(mp->devName, inFileSys);
        strcpy(mp->mntName, mount_point);

        //ino = getino();
        mip = iget(dev, ino);

        if(mip->INODE.i_mode != DIR_MODE){
            printf("-=0=]{ERROR Mount Point is NOT A DIR!\n");
            return -1;
        }

        for(i=0; i < NPROC; i++){
            if(proc[i].cwd == mip){
                printf("-=0=]{ERROR MOUNT POINT IS BUSY\n");
                return -1;
            }
        }

        mp->mntDirPtr = mip;
        mip->mptr = mp;
        mip->mounted = 1;
    }
    return 0;
}


int umount(char *filesys)
{
    int i;
    int mnt_index;
    MINODE *mip;

    //!Make sure its mounted
    for(i=0; i < NMTABLE; i++)
    {
        if(strcmp(mntTable[i].mntName, filesys) == 0){
            mnt_index = i;
            break;
        }
    }


    if(mnt_index == NMTABLE){
        printf("-=0={ERROR, filesys=%s, is NOT MOUNTED!\n", filesys);
        return -1;
    }

    //!Check if file system is busy
    for(i=0; i < NMINODE; i++){
        if(minode[i].dev == mntTable[mnt_index].dev){
            printf("-=0=[ERROR: Filesystem is busy!!\n");
            return -1;
        }
    }

    mip = mntTable[mnt_index].mntDirPtr;
    mip->mounted = 0;
    iput(mip);
    close(mntTable[mnt_index].dev);
    mntTable[mnt_index].dev = 0;
    return 0;
}