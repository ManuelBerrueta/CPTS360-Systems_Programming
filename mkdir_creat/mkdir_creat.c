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

int balloc(int dev) // allocate an inode number
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
            //decFreeInodes(dev); //TODO: replace with decFreeBlocks
            decFreeBlocks(dev);
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

    //1. pip points at the parent minode[] of "/a/b", name is a string "c") 

    //2. allocate an inode and a disk block for the new directory;
    //        ino = ialloc(dev);    
    //        bno = balloc(dev);
    //    DO NOT WORK IN THE DARK: PRINT OUT THESE NUMBERS!!!
    int ino = ialloc(dev);
    int bno = baloc(dev);

    printf("-=0={ NEW ALLOCATED: inode: %d  |  block: %d }=0=-", ino, bno);

    //3. mip = iget(dev, ino);  load the inode into a minode[] (in order to
    //   wirte contents to the INODE in memory.

    mip = iget(dev,ino); //? Load new allocated inode into a memory inode ???

    //4. Write contents to mip->INODE to make it as a DIR INODE.

    //5. iput(mip); which should write the new INODE out to disk.

    // C CODE of (3), (4) and (5):
    //**********************************************************************
    mip = iget(dev,ino);
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
    dp->inode = pip;                 //TODO: Need to double check this
    dp->rec_len = 2;
    dp->name[0] = dp->name[1] = '.';
    put_block(dev, BLKSIZE, localbuff);


    7. Finally, enter name ENTRY into parent's directory by                //TODO: START HERE
                enter_name(pip, ino, name);


    8. int enter_name(MINODE *pip, int myino, char *myname)
    {
     For each data block of parent DIR do { // assume: only 12 direct blocks

         if (i_block[i]==0) BREAK;

    (1). get parent's data block into a buf[];
    
    (2). EXT2 DIR entries: Each DIR entry has rec_len and name_len. Each entry's
         ideal length is   

            IDEAL_LEN = 4*[ (8 + name_len + 3)/4 ]
    
         All DIR entries in a data block have rec_len = IDEAL_LEN, except the last
         entry. The rec_len of the LAST entry is to the end of the block, which may
         be larger than its IDEAL_LEN.

      --|-4---2----2--|----|---------|--------- rlen ->------------------------|
        |ino rlen nlen NAME|.........|ino rlen nlen|NAME                       |
      --------------------------------------------------------------------------

    (3). To enter a new entry of name with n_len, the needed length is

            need_length = 4*[ (8 + n_len + 3)/4 ]  // a multiple of 4

    (4). Step to the last entry in a data block (HOW?).
    
        // get parent's ith data block into a buf[ ] 

           get_block(parent->dev, parent->INODE.i_block[i], buf);
    
           dp = (DIR *)buf;
           cp = buf;

           // step to LAST entry in block: int blk = parent->INODE.i_block[i];

           printf("step to LAST entry in data block %d\n", blk);
           while (cp + dp->rec_len < buf + BLKSIZE){

              /*************************************************
                 print DIR record names while stepping through
              **************************************************/

              cp += dp->rec_len;
              dp = (DIR *)cp;
           } 
           // dp NOW points at last entry in block
    
         Let remain = LAST entry's rec_len - its IDEAL_LENGTH;

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
      --------------------------------------------------------------------------

}

(5).// Reach here means: NO space in existing data block(s)

  Allocate a new data block; INC parent's isze by BLKSIZE;
  Enter new entry as the first entry in the new data block with rec_len=BLKSIZE.

  |-------------------- rlen = BLKSIZE -------------------------------------
  |myino rlen nlen myname                                                  |
  --------------------------------------------------------------------------

(6).Write data block to disk;
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