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

    //*dirname2 is second argument paased to link in main
    newino = getino(dirname2);
    if(newino == 0) //* newfile must not exist!
    {
        printf("=={ %s Does Not Exist\n", dirname2);
        puts("=={ Check Passed = Ready to check dname exists for new link()\n");
    }
    else
    {
        printf("=={ ERROR: File %s Exists = FAIL link()\n");
        //TODO: must put ino back?
        iput(oldmip);
        return 0;
    }
    

    //*Split the new file path into bname and dname
    char newPath[64] = { 0 };
    strcpy(newPath, dirname2);

    dbname(newPath);//Split parent dir name and the new file to be name
    
    //*Get ino of the Parent dir of the new file to be created
    int newDirtPathIno = getino(dname);
    MINODE *parentmip = iget(dev, newDirtPathIno);
    if(!S_ISDIR(parentmip->INODE.i_mode)) 
    {
        printf("\n-={0  parentmip '%s' is NOT a DIR. MUST be a parent DIR for new file: %s0}=-\n", dname, bname);
        printf("-={0  link() FAILED  0}=-\n") ;
        iput(oldmip);
        return 0;
    }

    enter_name(parentmip, oldino, bname); //*bname is the child name

    oldmip->INODE.i_links_count++;
    oldmip->dirty = 1; //*it has been modified, therefore is now dirty

    iput(oldmip);
    iput(parentmip);
}


int truncate()
{
    
}


int unlink()
{
    int deleteino = getino(pathname); //Get the inode number for the path
    MINODE *deletemip = iget(dev, deleteino);

    if(S_ISDIR(deletemip->INODE.i_mode)) 
    {
        printf("\n-={0  deletetmip '%s' MUST NOT be a DIR  0}=-\n", pathname);
        printf("-={0  UNLINK FAILED  0}=-\n") ;
        deletemip->INODE.i_links_count--;


        iput(deletemip);
        return;
    }

}

int symlink()
{
    //TODO: Create new file and change to link type to file type 
}

int readlink()
{

}