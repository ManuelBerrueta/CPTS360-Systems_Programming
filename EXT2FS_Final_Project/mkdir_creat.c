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
int bno = -1;



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

int decFreeBlocks(int dev)
{
    char buf[BLKSIZE];
    // dec free inodes count by 1 in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count--;
    put_block(dev, 2, buf);
}

int balloc(int dev) // allocate a block number
{
    int i;
    char buf[BLKSIZE];

    // read inode_bitmap block
    get_block(dev, bmap, buf);

    for (i = 0; i < nblocks; i++)
    {
        if (tst_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            put_block(dev, bmap, buf);
            decFreeBlocks(dev);

            //clear buffer
            memset(buf,0,BLKSIZE);
            put_block(dev, i+1, buf); //write back to block
            return i + 1;
        }
    }
    return 0;
}

// WRITE YOUR OWN int balloc(dev) function, which returns a FREE disk block number


//?================================= mkdir ===================================
int dbname(char *pathname)
{
    //! dirname() and basename() from libgen.h destroy the pathname!
    char temp[256];
    strcpy(temp, pathname); //* Make a copy of the path name
    strcpy(dname, dirname(temp));
    strcpy(temp, pathname);
    strcpy(bname, basename(temp));
}


int enter_name(MINODE *pip, int myino, char *myname)
{
    char buf[BLKSIZE];
    //8. int enter_name(MINODE *pip, int myino, char *myname){
    // For each data block of parent DIR do { // assume: only 12 direct blocks
    //    if (i_block[i]==0) BREAK;
    char *cp;
    int blk = -1;

    int i = 0;
    while (i < 12)
    {
        //* Looking for empty data block to enter name in parent's inode
        if(pip->INODE.i_block[i] == 0) 
        {
            int new_block = balloc(dev); //TODO: Up to here we should be goood
            /* (5).// Reach here means: NO space in existing data block(s)

                Allocate a new data block; INC parent's size by BLKSIZE;
                Enter new entry as the first entry in the new data block with rec_len=BLKSIZE.

                |-------------------- rlen = BLKSIZE -------------------------------------
                |myino rlen nlen myname                                                  |
                -------------------------------------------------------------------------- */
            dp->rec_len =BLKSIZE;
            //* Enter new DIR entry information
            strncpy(dp->name, myname, strlen(myname)); //Copy name into dp->name
            dp->inode = myino;                     
            dp->name_len = strlen(myname);
            //(6).Write data block to disk;
            put_block(dev, new_block, buf); //! Writes block back to disk
            return;
            //break; //* Once found empty data block break
        }
        
        //(1). get parent's data block into a buf[];

        get_block(pip->dev, pip->INODE.i_block[i], buf); //TODO: TEsting
        

        //*dp is now pointing to the first dir entry in the parent directory
        dp = (DIR *)buf;
        //char *cp = buf;
        cp = buf;

        //(2). EXT2 DIR entries: Each DIR entry has rec_len and name_len. Each entry's
        // ideal length is IDEAL_LEN = 4*[ (8 + name_len + 3)/4 ]
        //int IDEAL_LEN = 4*((8 + dp->name_len + 3)/4); //Current dir name len

        // All DIR entries in a data block have rec_len = IDEAL_LEN, except the last
        // entry. The rec_len of the LAST entry is to the end of the block, which may
        // be larger than its IDEAL_LEN.

   //*--|-4---2----2--|----|---------|--------- rlen ->------------------------|
   //*  |ino rlen nlen NAME|.........|ino rlen nlen|NAME                       |
   //*--------------------------------------------------------------------------


        // step to LAST entry in block: int blk = parent->INODE.i_block[i];
        //int blk = pip->INODE.i_block[i];
        blk = pip->INODE.i_block[i];
    
        printf("step to LAST entry in data block %d\n", blk);
        while (cp + dp->rec_len < buf + BLKSIZE)
        {
            /*************************************************
                print DIR record names while stepping through
            **************************************************/
            printf("At DIR record %s", dp->name); //*May need to fix this localy DIR* dp

            cp += dp->rec_len;
            dp = (DIR *)cp;
        } 
        //! dp from above loop NOW points at last entry in block

/*         Let remain = LAST entry's rec_len - its IDEAL_LENGTH;

            if (remain >= need_length){
            enter the new entry as the LAST entry and trim the previous entry
            to its IDEAL_LENGTH; 
            goto (6) below.
            } 

                                    EXAMPLE:

                                        |LAST entry 
        --|-4---2----2--|----|---------|--------- rlen ->------------------------|
        |ino rlen nlen NAME|.........|ino rlen nlen|NAME                       |
        --------------------------------------------------------------------------
                                                        |     NEW entry
        --|-4---2----2--|----|---------|----ideal_len-----|--- rlen=remain ------|
        |ino rlen nlen NAME|.........|ino rlen nlen|NAME|myino rlen nlen myname|
        -------------------------------------------------------------------------- */
        int IDEAL_LEN = 4*((8 + dp->name_len + 3)/4);
        int NEEDED_LEN = strlen(myname);
        int CURRENT_LEN = dp->rec_len;
        int REMAINING_LEN = CURRENT_LEN - IDEAL_LEN; //(4*((8 + IDEAL_LEN + 3)/4));

        if (REMAINING_LEN >= IDEAL_LEN)
        {
            //*Trim the size of the previous entry to its IDEAL_LEN
            dp->rec_len = IDEAL_LEN;
            //*Move up to the empty space to enter new dir
            //*We do this by moving up the new dp->rec_len of the current last DIR entry
            cp += dp->rec_len;
            dp = (DIR *)cp;
            //* New entry size will now be the last entry,
            //* Inherits the leftover size of rest of the block
            dp->rec_len = REMAINING_LEN;
            //* Enter new DIR entry information
            strncpy(dp->name, myname, NEEDED_LEN); //Copy name into dp->name
            dp->inode = myino;                     
            dp->name_len = NEEDED_LEN;
            //(6).Write data block to disk;
            put_block(dev, blk, buf); //! Writes block back to disk -need to do it back in my_mkdir
            //TODO: Should probably return here, we are done making new dir
            return;
        }
        i++;
    }
}


int mymkdir(MINODE *pip, char *name)
{
    MINODE *mip;

    //1. pip points at the parent minode[] of "/a/b", name is a string "c") 

    //2. allocate an inode and a disk block for the new directory;
    //        ino = ialloc(dev);    
    //        bno = balloc(dev);
    //    DO NOT WORK IN THE DARK: PRINT OUT THESE NUMBERS!!!
    int ino = ialloc(dev);
    bno = balloc(dev);

    printf("-=0={ NEW ALLOCATED: inode: %d  |  block: %d }=0=-\n", ino, bno);

    //3. mip = iget(dev, ino);  load the inode into a minode[] (in order to
    //   wirte contents to the INODE in memory.

    mip = iget(dev,ino); //? Load new allocated inode into a memory inode ???

    //4. Write contents to mip->INODE to make it as a DIR INODE.

    //5. iput(mip); which should write the new INODE out to disk.

    // C CODE of (3), (4) and (5):
    //**********************************************************************
    //mip = iget(dev,ino);
    INODE *ip = &mip->INODE;
    //Use ip-> to acess the INODE fields:
    ip->i_mode = 0x41ED;		// OR 040755: DIR type and permissions
    ip->i_uid  = running->uid;	// Owner uid 
    ip->i_gid  = running->pid;  //running->gid;	// Group Id
    ip->i_size = BLKSIZE;		// Size in bytes 
    ip->i_links_count = 2;	        // Links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);  // set to current time
    ip->i_blocks = 2;                	// LINUX: Blocks count in 512-byte chunks 
    ip->i_block[0] = bno;             // new DIR has one data block
    
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
    dp->inode = ino;
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
    enter_name(pip, ino, name);
    //put_block(dev, bno, localbuff); //! Writes block back to disk 
    //TODO: Possibly make localbuff a global?
}   
//!!END  OF mymkdir


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

    //TODO: This breaks if only one dir, need to deal with that case
/*     strcpy(gpath, pathname); //! Did not work...
    char *parent = dirname(gpath); 
    strcpy(gpath, pathname);
    char *child = basename(gpath); */

    dbname(pathname);

    //3. Get the In_MEMORY minode of parent:
             //pino  = getino(parent);
             //pip   = iget(dev, pino); 
    //int parentinode = getino(parent);
    int parentinode = getino(dname);
    MINODE *parentmip = iget(dev, parentinode);

    //Verify : (1). parent INODE is a DIR (HOW?)   AND
    //         (2). child does NOT exists in the parent directory (HOW?);
    if(!S_ISDIR(parentmip->INODE.i_mode)) 
    {
        printf("-={0  parentmip is NOT a DIR  0}=-\n");
        printf("-={0  mkdir FAILED  0}=-\n") ;
        return;
    }
    
    //if(search(parentmip, child) != 0) //!search returns 0 if child doesn't exists
    if(search(parentmip, bname) != 0) //!search returns 0 if child doesn't exists
    {
        //printf("-={0  child %s EXISTS 0}=-\n", child);
        printf("-={0  child %s EXISTS 0}=-\n", bname);
        printf("-={0  mkdir FAILED  0}=-\n") ;
        return;
    }
    
    //TODO: 4. 5. and 6.
    //! 4. call mymkdir(pip, child);
    //mymkdir(parentmip, child);
    mymkdir(parentmip, bname);

    //!5. inc parent inodes's link count by 1; 
       //* touch its atime and mark it DIRTY
    parentmip->INODE.i_links_count++;

    //6. iput(pip); //TODO: iput description
    iput(parentmip);
} 


int my_creat(MINODE *pip, char*name)//TODO: NEED TO FIX THIS...
{
    MINODE *mip;

    //1. pip points at the parent minode[] of "/a/b", name is a string "c") 

    //2. allocate an inode and a disk block for the new directory;
    //        ino = ialloc(dev);    
    int ino = ialloc(dev);
    //bno = balloc(dev); TODO: Don't need this because its a file

    printf("-=0={ NEW ALLOCATED: inode: %d  }=0=-\n", ino);

    //3. mip = iget(dev, ino);  load the inode into a minode[] (in order to
    //   write contents to the INODE in memory.

    mip = iget(dev,ino); //? Load new allocated inode into a memory inode ???

    //4. Write contents to mip->INODE to make it as a REG File INODE.

    //5. iput(mip); which should write the new INODE out to disk.

    //**********************************************************************
    //mip = iget(dev,ino);
    INODE *ip = &mip->INODE;
    //Use ip-> to acess the INODE fields:
    ip->i_mode = 0x81A4;		// OR 0100644: REG File type and permissions
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
    dp->inode = ino;
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
    enter_name(pip, ino, name);
    //put_block(dev, bno, localbuff); //! Writes block back to disk 
    //TODO: Possibly make localbuff a global?
}


int creat_file(char pathname[])
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

    //TODO: This breaks if only one dir, need to deal with that case
/*     strcpy(gpath, pathname); //! Did not work...
    char *parent = dirname(gpath); 
    strcpy(gpath, pathname);
    char *child = basename(gpath); */

    dbname(pathname);

    //3. Get the In_MEMORY minode of parent:
             //pino  = getino(parent);
             //pip   = iget(dev, pino); 
    //int parentinode = getino(parent);
    int parentinode = getino(dname);
    MINODE *parentmip = iget(dev, parentinode);

    //Verify : (1). parent INODE is a DIR (HOW?)   AND
    //         (2). child does NOT exists in the parent directory (HOW?);
    if(!S_ISDIR(parentmip->INODE.i_mode)) 
    {
        printf("-={0  parentmip is NOT a DIR  0}=-\n");
        printf("-={0  mkdir FAILED  0}=-\n") ;
        return;
    }
    
    //if(search(parentmip, child) != 0) //!search returns 0 if child doesn't exists
    if(search(parentmip, bname) != 0) //!search returns 0 if child doesn't exists
    {
        //printf("-={0  child %s EXISTS 0}=-\n", child);
        printf("-={0  child %s EXISTS 0}=-\n", bname);
        printf("-={0  mkdir FAILED  0}=-\n") ;
        return;
    }
    
    //TODO: 4. 5. and 6.
    //! 4. call mymkdir(pip, child);
    //mymkdir(parentmip, child);
    my_creat(parentmip, bname);

    //#For a file we do not increment parents link count

    //TODO: Description
    iput(parentmip);
}