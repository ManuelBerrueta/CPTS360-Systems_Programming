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

int my_linkcreat(MINODE *pip, char *name, int inoToLink)
{
    //1. pip points at the target minode[] of "/a/b/x" where is is target file
    
    MINODE *mip;

    //2. allocate an inode and a disk block for the new directory; //! We don't allocate a new inode because we use t inoToLink
    //        ino = ialloc(dev);    
    //int ino = ialloc(dev);//TODO: REPLACED this with inoToLink
    //bno = balloc(dev); TODO: Don't need this because its a file

    printf("-=0={ TARGET inode: %d  }=0=-\n", inoToLink);

    //3. mip = iget(dev, ino);  load the inode into a minode[] (in order to
    //   write contents to the INODE in memory.

    mip = iget(dev,inoToLink); //? Load new allocated inode into a memory inode ???

    //4. Write contents to mip->INODE to make it as a REG File INODE.

    //5. iput(mip); which should write the new INODE out to disk.

    //**********************************************************************
    //mip = iget(dev,ino);
    INODE *ip = &mip->INODE;
    //Use ip-> to acess the INODE fields:
    ip->i_mode = 0x1D4C0;		// OR 0100644: REG File type and permissions //TODO: FIND LINK CODE
    ip->i_uid  = running->uid;	// Owner uid 
    ip->i_gid  = running->pid;  //running->gid;	// Group Id
    ip->i_size = BLKSIZE;		// Size in bytes TODO: Do we just put 0?
    ip->i_links_count = 1;	    // Links count=1 because it is a file entry
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);  // set to current time
    ip->i_blocks = 1;                	// LINUX: Blocks count in 512-byte chunks 
    ip->i_block[0] = 0;             // new DIR has one data block
    //TODO: Check if i_blocks,i_block[0], and i_size are properly set
    
    int i=1;
    //ip->i_block[1] to i_block[14] = 0;
    while(i < 15)
    {
        ip->i_block[i] = 0;
        i++;
    }

    mip->dirty = 1;               // mark minode dirty
    iput(mip);                    // write INODE to disk


    //***** create data block for new DIR containing . and .. entries ******
    //6. Write . and .. entries into a buf[ ] of BLKSIZE

    //    | entry .     | entry ..                                             |
        //----------------------------------------------------------------------
        //|ino|12|1|.   |pino|1012|2|..                                        |
        //----------------------------------------------------------------------

    //    Then, write buf[ ] to the disk block bno;

    char localbuff[BLKSIZE] = { 0 };
    bzero(localbuff, BLKSIZE);

    dp = (DIR *)localbuff;
    
    //make the first entry of .
    dp->inode = inoToLink;
    dp->rec_len = 12;
    dp->name_len = 1;
    dp->name[0] = '.';

    //make second entry for ..
    //! NOTE: pino = parent DIR inpo, blk=allocated block
    dp = (char *)dp + 12; //* Moving 12 bytes (the size of the above dir) to next dir
    dp->inode = pip->ino;                 //TODO: Need to double check this    
    dp->rec_len = BLKSIZE - 12; //!@ this time this dir will span the rest of the block
    dp->name_len = 2;
    dp->name[0] = dp->name[1] = '.';
    
    put_block(dev, bno, localbuff); //! Writes block back to disk


    //7. Finally, enter name ENTRY into parent's directory by 
    enter_name(pip, inoToLink, name);
    //put_block(dev, bno, localbuff); //! Writes block back to disk 
    //TODO: Possibly make localbuff a global?
}

int creat_link(int oldino)
{
    MINODE *start = root;	
    MINODE *target_start = root;	     
    //1. pahtname = "/a/b/c" start = root;         dev = root->dev;
    //            =  "a/b/c" start = running->cwd; dev = running->cwd->dev;
    if(dirname2[0] != '/')
    {
        start = running->cwd;
    }
    if(pathname[0] != '/')
    {
        target_start = running->cwd;
    }
    //2. Let  
    //     parent = dirname(pathname);   parent= "/a/b" OR "a/b"  = dname
    //     child  = basename(pathname);  child = "c"              = bname
    //!  WARNING: strtok(), dirname(), basename() destroy pathname

    //TODO: This breaks if only one dir, need to deal with that case
/*     strcpy(gpath, pathname); //! Did not work...
    char *parent = dirname(gpath); 
    strcpy(gpath, pathname);
    char *child = basename(gpath); */

    char newPath[64] = { 0 };
    strcpy(newPath, dirname2);
    dbname(newPath);

    //3. Get the In_MEMORY minode of parent:
             //pino  = getino(parent);
             //pip   = iget(dev, pino); 
    //int parentinode = getino(parent);
    //! /x/y dir where link will be created
    int parentinode = getino(dname);
    MINODE *parentmip = iget(dev, parentinode);

    //*Get ino of target 
    int targetinode = getino(pathname);
    MINODE* targetmip = iget(dev, targetinode);

    //Verify : (1). parent INODE is a DIR (HOW?)   AND
    //         (2). child does NOT exists in the parent directory (HOW?);
    if(!S_ISDIR(parentmip->INODE.i_mode)) 
    {
        printf("-={0  parentmip is NOT a DIR  0}=-\n");
        printf("-={0 creat_link FAILED  0}=-\n") ;
        return;
    }
    if(S_ISDIR(targetmip->INODE.i_mode)) 
    {
        printf("-={0  targetmip is a DIR, link to a dirs is not allowed with link()  0}=-\n");
        printf("-={0 creat_link FAILED  0}=-\n") ;
        return;
    }

    
    //if(search(parentmip, child) != 0) //!search returns 0 if child doesn't exists
    if(search(parentmip, bname) != 0) //!search returns 0 if child doesn't exists
    {
        //printf("-={0  child %s EXISTS 0}=-\n", child);
        printf("-={0  Parent of link: %s ino#: EXISTS 0}=-\n", bname, parentmip->ino);
        printf("-={0  Continue...  0}=-\n") ;
    }

    //* Check the file we are trying to create does not exist!
    int newFileCheckino = getino(dirname2);
    if(newFileCheckino== 0)
    {
        printf("-={0  new File DNE 0}=-\n", bname, parentmip->ino);
        printf("-={0  Continue...  0}=-\n") ;
    }
    
    //TODO: Add an an try to /x/y that point to targetino

    my_linkcreat(parentmip, bname, oldino);

    //#For a file we do not increment parents link count

    //TODO: Description
    iput(parentmip);
}


int link()
{
    int oldino = getino(pathname); //Get the inode number for the path
    MINODE *oldmip = iget(dev, oldino);
    MINODE *newmip;
    int newino = -1;

    if(S_ISDIR(oldmip->INODE.i_mode)) 
    {
        printf("\n-={0  parentmip '%s' MUST NOT  be a DIR  0}=-\n", pathname);
        printf("-={0  LINK FAILED  0}=-\n") ;
        iput(oldmip);
        return;
    }

    newino = getino(dirname2);//*dirname2 is second dir passed in in main
    if(newino == 0)
    {
        printf("%s DNE, good ready to check dname exists for new link()\n", dirname2);
    }

    //*Split the new file path into bname and dname
    char newPath[64] = { 0 };
    strcpy(newPath, dirname2);

    dbname(newPath);
    //*Get ino of the path parent dir
    int newDirtPathIno = getino(dname);
    MINODE *parentmip = iget(dev, newDirtPathIno);
    if(!S_ISDIR(parentmip->INODE.i_mode)) 
    {
        printf("\n-={0  parentmip '%s' MUST be a parent DIR for  new link()  0}=-\n", pathname);
        printf("-={0  LINK FAILED  0}=-\n") ;
        iput(oldmip);
        return;
    }

    newino = oldino;
    //create link file
    creat_link(oldino);

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