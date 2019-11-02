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

char dname[64];         //? Directory string holder
char bname[64];         //? Basename string holder



int tst_bit(char *buf, int bit)
{
    int i, j;
    i = bit / 8;
    j = bit % 8;
    if (buf[i] & (1 << j))
        return 1;
    return 0;
}

int set_bit(char *buf, int bit)
{
    int i, j;
    i = bit / 8;
    j = bit % 8;
    buf[i] |= (1 << j);
}

int clr_bit(char *buf, int bit)
{
    int i, j;
    i = bit / 8;
    j = bit % 8;
    buf[i] &= ~(1 << j);
}

int decFreeInodes(int dev)
{
    char buf[BLKSIZE];
    // dec free inodes count by 1 in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count--;
    put_block(dev, 2, buf);
}

int ialloc(int dev) // allocate an inode number
{
    int i;
    char buf[BLKSIZE];

    // read inode_bitmap block
    get_block(dev, imap, buf);

    for (i = 0; i < ninodes; i++)
    {
        if (tst_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            put_block(dev, imap, buf);
            decFreeInodes(dev);
            return i + 1;
        }
    }
    return 0;
}

// WRITE YOUR OWN int balloc(dev) function, which returns a FREE disk block number


//?================================= mkdir ===================================
int mymkdir(MINODE *pip, char *name)
{
    MINODE *mip;
}




int make_dir()
{
    MINODE *start = root;		     
    //1. pahtname = "/a/b/c" start = root;         dev = root->dev;
    //            =  "a/b/c" start = running->cwd; dev = running->cwd->dev;
    if(pathname[0] != '/')
    {
        start = running->cwd;
    }
    //2. Let  
    //     parent = dirname(pathname);   parent= "/a/b" OR "a/b"  = dname
    //     child  = basename(pathname);  child = "c"              = bname
    //!  WARNING: strtok(), dirname(), basename() destroy pathname
    strcpy(gpath, pathname);
    char *parent = dirname(gpath);
    strcpy(gpath, pathname);
    char *child = basename(gpath);

    //3. Get the In_MEMORY minode of parent:
             //pino  = getino(parent);
             //pip   = iget(dev, pino); 
    int parentinode = getino(parent);
    MINODE *parentmip = iget(dev, parentinode);

    //Verify : (1). parent INODE is a DIR (HOW?)   AND
    //         (2). child does NOT exists in the parent directory (HOW?);
    if(!S_ISDIR(parentmip->INODE.i_mode))
    {
        printf("-={0  parentmip is NOT a DIR  0}=-\n");
        printf("-={0  mkdir FAILED  0}=-\n") ;
        return;
    }
    
    if(search(parentmip, child) != 0) //!search returns 0 if child doesn't exists
    {
        printf("-={0  child %s EXISTS 0}=-\n", child);
        printf("-={0  mkdir FAILED  0}=-\n") ;
        return;
    }
    
    //TODO: 4. 5. and 6.
    //! 4. call mymkdir(pip, child);
    mymkdir(parentmip, child);

    //!5. inc parent inodes's link count by 1; 
       //* touch its atime and mark it DIRTY
    parentmip->INODE.i_links_count++;

    //6. iput(pip); //TODO: iput description
    iput(parentmip);
} 