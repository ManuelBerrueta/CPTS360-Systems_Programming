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
    if(oldino == 0)
    {
        printf("=0={ERROR: File:%s to be symlinked Does Not Exist", pathname);
        return 0;
    }
    MINODE *oldmip = iget(dev, oldino);
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


int truncate(MINODE *mipToDelete)
{
    //* Deallocate all data blocks in INODE:
    for(int i=0; i < 12; i++)
    {
        if(mipToDelete->INODE.i_block[i] == 0)
        {
            break;
        }
        bdalloc(dev, mipToDelete->INODE.i_block[i]);
    }
    //* Deallocate INODE
    idalloc(dev, mipToDelete->ino);
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

    //TODO: Possibly move this 3 lines below the if
    //rm_child(parentmip, bname);
    //parentmip->dirty = 1;
    //iput(parentmip);
    
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
        //!use truncate function
        truncate(deletemip);
    }
    rm_child(parentmip, bname);
    parentmip->dirty = 1;
    iput(parentmip);
    iput(deletemip);

    //TODO: Bug due to the name...may be check name before deleting link?
}

int symlink()
{
    //TODO: Create new file and change to link type to file type 
    int oldino = getino(pathname); //Get the inode of file to be linked
    if(oldino == 0)
    {
        printf("=0={ERROR: File:%s to be symlinked Does Not Exist", pathname);
        return 0;
    }
    MINODE *oldmip = iget(dev, oldino);
    int newino = -1;
    //*If the file is not a DIR or a REG file throw error
    if(!S_ISDIR(oldmip->INODE.i_mode) && !S_ISREG(oldmip->INODE.i_mode))
    {
        printf("\n-={0  parentmip '%s' MUST BE a DIR or REG file  0}=-\n", pathname);
        printf("-={0  LINK FAILED: i_mode=%d  0}=-\n", oldmip->INODE.i_mode) ;
        iput(oldmip);
        return 0;
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
    char parentName[128] = {0};
    char childName[128] = {0};
    strcpy(parentName, dname);
    strcpy(childName, bname);

    int newDirtPathIno = getino(parentName);
    MINODE *parentmip = iget(dev, newDirtPathIno);
    if(!S_ISDIR(parentmip->INODE.i_mode)) 
    {
        printf("\n-={0  parentmip '%s' is NOT a DIR. MUST be a parent DIR for new file: %s0}=-\n", dname, bname);
        printf("-={0  link() FAILED  0}=-\n") ;
        iput(oldmip);
        return 0;
    }

    my_creat(parentmip, childName);
    newino = getino(dirname2);
    if(newino == 0)
    {
        printf("ERROR: INODE for the new file created was not found\n");
        iput(oldmip);
        return 0;
    }
    //TODO:creat_file
    //TODO: getino of new file
    //TODO: put it in memmory = mip
    //TODO: change the mode to 0XA000
    //TODO: Write the string of oldName into the i_block[] array
    //TODO: set file size to strlen(of oldName)
    //TODO: Write INODE of new file back to disk

    MINODE *newmip = iget(dev, newino);
    newmip->INODE.i_mode = 0xA000; //Set file type to link
    
    char oldFilePath[128] = {0};
    strcpy(oldFilePath, pathname);
    dbname(oldFilePath);
    char oldFileName[64] = {0};
    strcpy(oldFileName, bname);
    newmip->INODE.i_size = strlen(oldFileName);
    //*Write oldFileName into i_block[]
    strcpy((char*)newmip->INODE.i_block, oldFileName);//TODO: Possible issue here
    newmip->dirty = 1;
    iput(newmip);

    //enter_name(parentmip, oldino, bname); //*bname is the child name

    //oldmip->INODE.i_links_count++;
    parentmip->dirty = 1; //*it has been modified, therefore is now dirty

    iput(oldmip);
    iput(parentmip);
}

int readlink()
{
    int inoToCheck = getino(pathname);
    if(inoToCheck == 0)
    {
        printf("=0={ERROR: File:%s to readlink() Does Not Exist", pathname);
        return 0;
    }
    MINODE* checkIno = iget(dev, inoToCheck);
    if(!S_ISLNK(checkIno->INODE.i_mode))
    {
        printf("-=0={ ERROR: File:%s is NOT a symlink file\n", pathname);
        printf("-=0={ File's i_mode=%d", checkIno->INODE.i_mode);
        return 0;
    }

    char symlnkname[128] = { 0 };
    strcpy(symlink, checkIno->INODE.i_block); //TODO: Possible issues Here

    return strlen(symlink);
}