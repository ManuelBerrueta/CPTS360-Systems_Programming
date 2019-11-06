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

extern char line[256], command[32], pathname[256];

extern char dname[64]; //? Directory string holder
extern char bname[64]; //? Basename string holder
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
    get_block(dev, imap, buf);
    clr_bit(buf, ino - 1);

    // write buf back
    put_block(dev, imap, buf);

    // update free inode count in SUPER and GD
    incFreeInodes(dev);
}

int incFreeBlocks(int dev)
{
    char buf[BLKSIZE];

    // inc free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count++;
    put_block(dev, 2, buf);
}


int bdalloc(int dev, int blk) // deallocate a blk number
{
    int i;
    char buf[BLKSIZE];

    if (blk > nblocks)
    {
        printf("inumber %d out of range\n", blk);
        return;
    }

    // get inode bitmap block
    get_block(dev, bmap, buf);
    clr_bit(buf, blk - 1);

    // write buf back
    put_block(dev, bmap, buf);

    // update free inode count in SUPER and GD
    incFreeBlocks(dev);
}

// rm_child(): remove the entry [INO rlen nlen name] from parent's data block.

int rm_child(MINODE *parent, char *myname)
{
   //1. Search parent INODE's data block(s) for the entry of myname
    int i = 0;
    char buf[BLKSIZE];

    while( i < 12)
    {
        get_block(parent->dev, parent->INODE.i_block[i],buf);
        dp = (DIR *) buf;
        char *cp = buf;
        char *cp2;
        DIR *prevdp = dp;
        int rec=-1;

        while (cp < buf + BLKSIZE)
        {
            /*************************************************
                print DIR record names while stepping through
            **************************************************/
            printf("At DIR record %s", dp->name); //*May need to fix this localy DIR* dp

            if(strcmp(dp->name, myname) == 0) //!If this is the dir we are trying to delete
            {
                if(cp + dp->rec_len >= buf + BLKSIZE)
                {
                    prevdp->rec_len += dp->rec_len;
                    put_block(dev, parent->INODE.i_block[i], buf);
                    return;
                }
                else if(dp->rec_len == BLKSIZE)
                {
                    bdalloc(dev, parent->INODE.i_block[i]);

                    //!New code after working
/*                     for(i= +1; i < 12; i++)
                    {
                        if(parent->INODE.i_block[i] == 0)
                        {
                            parent->INODE.i_block[i-1] = 0;
                            break;
                        }
                        parent->INODE.i_block[i-1] = parent->INODE.i_block[i];
                    } */

                    return;
                }
                else
                {
                        cp2 = cp + dp->rec_len;
                        rec = dp->rec_len;
                        dp = (DIR *)cp2;
                        while (cp2 < buf + BLKSIZE)
                        {
                            memcpy(cp, cp2, dp->rec_len);
                            dp = (DIR *)cp;
                            cp2 += dp->rec_len;
                            cp += dp->rec_len;
                            dp = (DIR *)cp;
                        }
                        dp->rec_len += rec;
                        put_block(dev, parent->INODE.i_block[i], buf);
                        return 0;
                }    
            }
            cp += dp->rec_len;
            prevdp = dp;
            dp = (DIR *)cp;
        } 

/*         2. Erase name entry from parent directory by
            
        (1). if LAST entry in block{
                                                |remove this entry   |
                -----------------------------------------------------
                xxxxx|INO rlen nlen NAME |yyy  |zzz                 | 
                -----------------------------------------------------

                        becomes:
                -----------------------------------------------------
                xxxxx|INO rlen nlen NAME |yyy (add zzz len to yyy)  |
                -----------------------------------------------------

            }
            
        (2). if (first entry in a data block){
                deallocate the data block; modify parent's file size;

                -----------------------------------------------
                |INO Rlen Nlen NAME                           | 
                -----------------------------------------------
                
                Assume this is parent's i_block[i]:
                move parent's NONZERO blocks upward, i.e. 
                    i_block[i+1] becomes i_block[i]
                    etc.
                so that there is no HOLEs in parent's data block numbers
            }

        (3). if in the middle of a block{
                move all entries AFTER this entry LEFT;
                add removed rec_len to the LAST entry of the block;
                no need to change parent's fileSize;

                    | remove this entry   |
                -----------------------------------------------
                xxxxx|INO rlen nlen NAME   |yyy  |zzz         | 
                -----------------------------------------------

                        becomes:
                -----------------------------------------------
                xxxxx|yyy |zzz (rec_len INC by rlen)          |
                -----------------------------------------------

            }
            
        3. Write the parent's data block back to disk;
            mark parent minode DIRTY for write-back */

        i++;
    }
}



int rmdir()
{
    //2. get inumber of pathname : ino = getino(pathname)
    int ino = getino(pathname);

    //3. get its minode[] pointer : mip = iget(dev, ino);
    MINODE *mip = iget(dev, ino);

    //4. check ownership 
       //super user : OK
       //not super user: uid must match
    int ino_uid = mip->INODE.i_uid;
    int userid = running->uid;

    if(userid != 0)
    {
        if(userid != ino_uid)
        {
            printf("Error uid does not have permission to rmdir\n");
            iput(mip);
            return;
        }
    }
    

    //*Make sure that is not root we are trying to delete
    if(root->ino == ino)
    {
        printf("CANNOT DELETE ROOT!\n");
        iput(mip);
        return;
    }

    if((strcmp(pathname, ".") == 0) || (strcmp(pathname, "..") ==0))
    {
        printf("CANNOT DELETE %s\n", pathname);
        iput(mip);
        return;
    }


    //------------------------------------------------------------------------
    //5. check DIR type (HOW?), not BUSY (HOW?), is empty:

    if(!S_ISDIR(mip->INODE.i_mode)) 
    {
        printf("-={0  parentmip is NOT a DIR  0}=-\n");
        printf("-={0  mkdir FAILED  0}=-\n") ;
        iput(mip);
        return;
    }

    //HOW TO check whether a DIR is empty:
    //First, check link count (links_count > 2 means not empty);
    //However, links_count = 2 may still have FILEs, so go through its data block(s) to see whether it has any entries in addition to.and..
    int i = 0;

    while(i < 12)
    {
        if(mip->INODE.i_block[i] == 0)
        {
            break;
        }
        i++;
    }
    
    char buf[BLKSIZE] = {0};
    
    get_block(dev,mip->INODE.i_block[0], buf);

    dp = (DIR *) buf;
    char *cp = buf;

    i=0;
    while (cp < buf + BLKSIZE)
    {
        cp += dp->rec_len;
        dp = (DIR *)cp;
        i++;

        if(i > 2)
        {
            printf("DIR %s IS NOT EMPTY, CANNOT RMDIR!\n", pathname);
            iput(mip);
            return;
        }
    } 

    if(mip->refCount != 1)
    {
        printf("DIR is Busy , CANNOT RMDIR\n");
        iput(mip);
        return;
    }
    //if (NOT DIR || BUSY || not empty) : iput(mip);
    
    //7. get parent DIR's ino and Minode (pointed by pip);
         //pip = iget(mip->dev, parent's ino); 
    
    int pino = search(mip, "..");
    MINODE *parentmip = iget(dev, pino);

    char lmyname[256] = { 0 };
    findmyname(parentmip, ino, lmyname);

    //6. ASSUME it passed the above checks. Deallocate its block and inode 
    for (i = 0; i < 12; i++)
    {
        if (mip->INODE.i_block[i] == 0)
            continue;
        bdalloc(mip->dev, mip->INODE.i_block[i]);
    }
    idalloc(mip->dev, mip->ino);
    iput(mip); //(which clears mip->refCount = 0);

    //TODO: Why are we

    //8. remove child's entry from parent directory by rm_child(MINODE *pip, char *name);
            //pip->parent Minode, name = entry to remove
    rm_child(parentmip, lmyname);

    //9. decrement pip's link_count by 1; 
    //touch pip's atime, mtime fields;
    //mark pip dirty;

    parentmip->INODE.i_links_count--;

    parentmip->INODE.i_mtime = parentmip->INODE.i_atime = time(0L);

    parentmip->dirty = 1;
    iput(parentmip);
    //return SUCCESS;
    return 1;
}