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


//!   Example: link     /a/b/c                      /x/y/z ==>
//!                     /a/b/   datablock           /x/y    data block
//!                 ------------------------        -------------------------
//!                .. .|ino rlen nlen c|...        ....|ino rlen nlen z| ....
//!                ------|-----------------        ------|------------------
//!                      |                               |
//!                    INODE <----------------------------
//!
//!                i_links_count = 1  <== INCrement i_links_count to 2


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
    

    //*Split the new file path into dname and bname
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
    int inoToDelete = getino(pathname); //Get the inode number for the path

    if(inoToDelete == 0)
    {
        printf("\n-={0 ERROR: inoToDelete '%s' Does NOT Exist 0}=-\n", pathname);
        printf("-={0  UNLINK FAILED  0}=-\n") ;
        return 0;
    }

    MINODE *deletemip = iget(dev, inoToDelete);

    if(S_ISDIR(deletemip->INODE.i_mode)) 
    {
        printf("\n-={0 ERROR: deletetmip '%s' is a DIR. MUST NOT be a DIR  0}=-\n", pathname);
        printf("-={0  UNLINK FAILED  0}=-\n") ;
        iput(deletemip);
        return 0;
    }

    //*Split the link to delete path into dname and bname
    char newPath[64] = { 0 };
    strcpy(newPath, pathname);

    //* Remove entry from Parent's DIR's data block:
    int parentino = getino(dname);
    MINODE *parentmip = iget(dev, parentino);
    //rm_child(parentmip, inoToDelete, bname);
    rm_child(parentmip, bname);
    parentmip->dirty = 1;
    iput(parentmip);
    
    deletemip->INODE.i_links_count--;
    if(deletemip->INODE.i_links_count > 0)
    {
        // Mark it dirty to be rewritten back to disk
        deletemip->dirty = 1;
    }
    else //! if i_links_count == 0, then completely remove file
    {
        //TODO: deallocate all data blocks in INODE;
        //TODO: deallocate INODE
    }
    iput(deletemip);
}

int symlink()
{
    //TODO: Create new file and change to link type to file type 
}

int readlink()
{

}