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


/********** globals *************/
extern int fd, dev;                         // dev = fd
extern int ninodes, nblocks;                // ninodes, nblocks numbers from SUPER
extern int bmap, imap, iblock, inode_start; // BMAP, IMAP, inodes start block numbers

extern char gpath[256]; // token strings
extern int n;           // number of token strings
extern char *name[64];  // pointers to token strings

extern char line[256], command[32], pathname[256];

extern char dname[64];         //? Directory string holder
extern char bname[64];         //? Basename string holder
extern int bno;


int incFreeInodes(int dev)
{
    char buf[BLKSIZE];

    // inc free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count++;
    put_block(dev, 2, buf);
}

idalloc(int dev, int ino) // deallocate an ino number
{
    int i;
    char buf[BLKSIZE];

    if (ino > ninodes)
    {
        printf("inumber %d out of range\n", ino);
        return;
    }

    // get inode bitmap block
    get_block(dev, bmap, buf);
    clr_bit(buf, ino - 1);

    // write buf back
    put_block(dev, imap, buf);

    // update free inode count in SUPER and GD
    incFreeInodes(dev);
}

int bdalloc(int dev, int blk) // deallocate a blk number
{
    // WRITE YOUR OWN CODE to deallocate a block number blk
}