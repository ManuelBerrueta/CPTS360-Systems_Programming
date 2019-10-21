/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 **                  WSU CPTS 360-SYSTEMS PROGRAMMING                       **
 **                         LAB 5 - ShownBlock                              **
 **                                by                                       **
 **                          Manuel Berrueta                                **
 **                                                                         **
 **     Summary: Program that displays the disk blocks of a file            **
 **              in an EXT2 file system.The program runs as follows:        **
 **                                                                         **
 **         showblock    DEVICE    PATHNAME                                 **
 **         ---------  ---------  -----------                               **
 **  e.g.   showblock  diskimage  /a/b/c/d  (diskimage contains an EXT2 FS) **
 **                                                                         **
 **  It locates the file named PATHNAME and prints the disk blocks (direct, **
 **  indirect, double-indirect) of the file.                                **
 **                                                                         **
 **     To build: cc main.c -o showdisk -m32                                **
 **     Requires EXT2 File System libs: sudo apt install e2fslibs-dev       **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <ext2fs/ext2_fs.h>


//! Define shorter types, to save fingers =)
typedef struct ext2_group_desc    GD;
typedef struct ext2_super_block   SUPER;
typedef struct ext2_inode         INODE;
typedef struct ext2_dir_entry_2   DIR;

#define BLKSIZE 1024

GD      *gp;
SUPER   *sp;
INODE   *ip;
DIR     *dp;

//char *dev = "mydisk";
char buf[1024], dbuf[1024], sbuf[256], devName[256];
char indirect_blk_buff[1024], double_indirect_blk_buff[1024];

int dev;//! File Descriptor for Device
int bmap, imap, iblock;
int rootblock;

// Functions
int tokenize(char inStrToTok[], char *retCharArr[]);
int get_block(int dev, int blk, char *buff);
void *dir(char *devName, char pathName[]);
int search(INODE *ip, char name[]);
void *show_dir(INODE *ip);
void *printIPinfo(char fileName[], INODE *ip);
INODE *path2inode(INODE *ip, char pathName[], int inodes_start);

int main(int argc, char const *argv[])
{
    char pathName[256] = { 0 };
    if (argc > 1)
    {
        strcpy(devName, argv[1]);
        if(argv[2] != 0)
        {
            strcpy(pathName, argv[2]);
        }
    }
    else
    {
        strcpy(devName, "diskimage");
        strcpy(pathName, "/Z/hugefile");
    }
    dir(devName, &pathName);

    return 0;
}


//TODO: Tokenize function, it will return a char *array[]
int tokenize(char inStrToTok[], char *retCharArr[])
{
    int numOfComponents =0;
    
    if(inStrToTok == 0)
    {
        printf("NULL STRING - Can't Tokenize Empty String!\n\n");
        return -1;
    }

    char* tempPath;
    retCharArr[numOfComponents++] = strtok(inStrToTok, "/");

    while((tempPath = strtok(NULL, "/")))
    {
        retCharArr[numOfComponents++] = tempPath;
    }

    return numOfComponents;
}


// Reads a disk BLOCK into a global char buff[BLKSIZE]
int get_block(int dev, int blk, char *buff)
{
    int n = -1;
    //* Move to an offset of block_number * BLKSIZE
    lseek(dev, blk * BLKSIZE, SEEK_SET);
    //* One at that offset read the block into the buffer
    n = read(dev, buff, BLKSIZE); //Using global buf

    if(n < 0)
    {
        printf("ERROR: Failed get_block %d\n", blk);
    }

    return n;
}


void *dir(char *devName, char pathName[])
{
    int i = 0;
    char *cp;

    //! (1) Open diskimage for READ or RDWR
    dev = open(devName, O_RDONLY);
    if(dev < 0)
    {
        printf("FAILED to open %s\n", devName);
        exit(1);
    }
    // Read SUPER Block at given offset 
    int blockNum = 1;
    get_block(dev, blockNum, buf); //*Read SUPER Block (Block  #1)

    //! (2) Read in SUPER block (block #1) to verify diskimage is an EXT2 FS
    sp = (SUPER *)buf; //* Casting the buffer as a SUPER Block
    //* Checking magic number to make it is a EXT2 File System:
    printf("**Checking to make sure %s is a EXT2 File System\n", devName);
    if(sp->s_magic != 0xEF53) //*Checking magic# matches magic number of EXT2 FS
    {
        printf("**%s is NOT a EXT2 File System!\n",devName);
        exit;
    }
    else
    {
        printf("****%s verified to be an EXT2 File System \n");
    }
    
    //?(3) Read Group Descriptor block (block #2)
    //? Get block # of bmap, imap, inodes_start and print their values.
    get_block(dev, 2, buf); //! move up to Block#2 and read into global buf
    gp = (GD *) buf; //! Cast the buf as a group descriptor

    puts("\n-=0=-=0=-=0=-=0={ Group Descriptor (GD) info: }=0=-=0=-=0=-=0=- ");
    puts("map | imap | iblock | # free blocks | # free inodes | dir count");
    printf(" %d      %d      %d          %d             %d           %d\n",
            gp->bg_block_bitmap,
            gp->bg_inode_bitmap,
            gp->bg_inode_table,
            gp->bg_free_blocks_count,
            gp->bg_free_inodes_count,
            gp->bg_used_dirs_count);

    bmap = gp->bg_block_bitmap;
    imap = gp->bg_inode_bitmap;
    iblock = gp->bg_inode_table;
    printf("\tGD Summary: bmap = %d | imap = %d | iblock = %d\n\n", bmap, imap, iblock);
    int inodes_start=iblock; //!Tentative got to double check this

    //* Read first INODE block to get root inode #2
    //?(4) let INODE *ip
    get_block(dev, iblock, buf);
    ip = (INODE *)buf;
    ip++;

    printf("************{ root inode info }************\n");
    printf("mode=%4x | uid=%d | gid=%d\n", ip->i_mode, ip->i_uid, ip->i_gid);
    printf("size=%d\n", ip->i_size);
    printf("time=%s", ctime(&ip->i_ctime));
    printf("link=%d\n", ip->i_links_count);
    printf("i_block[0]=%d\n", ip->i_block[0]);
    rootblock=ip->i_block[0];
    puts("*******************************************\n\n");
    

    puts("************{ root directory contents }************");
    show_dir(ip);
    puts("***************************************************\n\n");
    //getchar(); //! breakpoint

    //!Reset back to root
    get_block(dev, inodes_start, buf);
    ip = (INODE *)buf;
    ip++;
    //show_dir(ip);

    ip = path2inode(ip, pathName, inodes_start);

    int j = 0;
    if(ip->i_block[12] > 0)
    {
        //TODO: search indirect blocks
        puts("\n-=0=-=0=-=0=-=0={ Indirect Blocks }=0=-=0=-=0=-=0=- ");
        get_block(dev, ip->i_block[12], indirect_blk_buff);
        int * ind_blk_buf_ptr = (int *)indirect_blk_buff;
        for(i=0; i < 256; i++)
        {
            if(ind_blk_buf_ptr[i] == 0)
            {
                break;
            }
            printf("%d ", ind_blk_buf_ptr[i]);
            fflush(stdout);
        }
        puts("\n-=0=-=0=-=0=-=0==0=-=0=-=0=-=0=-=0=-=0=-=0=-=0=- ");
    }
    if(ip->i_block[13] > 0)
    {
        //TODO: search double indirect blocks
        get_block(dev, ip->i_block[13], indirect_blk_buff);
        int * ind_blk_buf_ptr = (int *)indirect_blk_buff;
        puts("\n-=0=-=0=-=0=-=0={ Double Indirect Blocks }=0=-=0=-=0=-=0=- ");
        for(i=0; i < 256; i++)
        {
            if(ind_blk_buf_ptr[i] == 0)
            {
                break;
            }
            get_block(dev,ind_blk_buf_ptr[i], double_indirect_blk_buff);
            int * doub_ind_blk_buf_ptr = (int *)double_indirect_blk_buff;
            for(j=0; j < 256; j++)
            {
                if(doub_ind_blk_buf_ptr[j] == 0)
                {
                    break;
                }
                printf("%d ", doub_ind_blk_buf_ptr[j]);
                fflush(stdout);
            }
        }
        puts("\n-=0=-=0=-=0=-=0==0=-=0=-=0=-=0=-=0=-=0=-=0=-=0=-=0=-=0=-=0=- ");
    }
    if(ip->i_block[14] > 0)
    {
        //TODO: Search Triple indirect blocks
        //* NOT REQUIRED FOR FINAL PROJECT
    }





/*  
    char *tokenizedPath[64];
    int numOfComponents=0;
    
    if(pathName != 0) //* If a pathname was passed
    {
        numOfComponents = tokenize(pathName, tokenizedPath);
        int j=0;
        
        //TODO:For debugging only
        printf("Tokenized path:> ");
        while(j < numOfComponents)
        {
            printf("/%s", tokenizedPath[j++]);
            fflush(stdout);
        }
        puts("");
        //TODO: Search will go here --!NOTE: may need to do ip
        int ino, blk, offset;
        int n = numOfComponents; 

        for (i=0; i < n; i++)
        {
            ino = search(ip, tokenizedPath[i]);
        
            if (ino==0)
            {
                printf("can't find %s\n", tokenizedPath[i]); 
                exit(1);
            }
             // Mailman's algorithm: Convert (dev, ino) to INODE pointer
            blk    = (ino - 1) / 8 + inodes_start; 
            offset = (ino - 1) % 8;        
            get_block(dev, blk, buf);
            ip = (INODE *)buf + offset;   // ip -> new INODE
        }
        
        //*Print information out of current ip
        printIPinfo(tokenizedPath[i-1], ip);

        if(ip->i_block[12] > 0)
        {
            //TODO: search indirect blocks
        }
        if(ip->i_block[13] > 0)
        {
            //TODO: search double indirect blocks
        }
        if(ip->i_block[14] > 0)
        {
            //TODO: search Triple indirect blocks
        }

        
        //printf("%s", ip->)
    }
    else
    {
        //show_dir(ip);
    } */

}


//* Search directory's data blocks for a name string
//* return its inode number if found, if not found return 0.
int search(INODE *ip, char name[])
{
    char temp[256];
    DIR *dp;
    char *cp;
    int ino, block, offset;
    int i=0;

    for(i=0; i < 12; i++)
    {
        if (ip->i_block[i] == 0)
        {
            break;
        }
        get_block(dev, ip->i_block[i], sbuf);
        dp = (DIR *)sbuf;
        cp = sbuf;

        while(cp < sbuf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0; //add null char to the end off the dp->name

            //TODO: strcmp to see if the given name exists
            if (strcmp(&name[i], temp) == 0)
            {
                printf("*****={ inode %s found, inode# = %d }=*****\n", name, dp->inode);
                printf("%4d       %4d      %4d        %s\n", 
                dp->inode, dp->rec_len, dp->name_len, temp);
                return dp->inode;
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }
    printf("**inode %s, not found in data blocks\n\n", name);
    return 0;
}

void *show_dir(INODE *ip)
{
    //char sbuf[BLKSIZE], char temp[256];
    char temp[256];
    DIR *dp;
    char *cp;
    int i;

    for (i=0; i < 12; i++) //* assume DIR at most 12 direct blocks
    {  
        if (ip->i_block[i] == 0)
        {
            break;
        }

        // YOU SHOULD print i_block[i] number here
        get_block(dev, ip->i_block[i], sbuf);
        dp = (DIR *)sbuf;
        cp = sbuf;

        puts(" Inode |   Size   | FName Size | File Name");

        while(cp < sbuf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            printf("%4d       %4d      %4d        %s\n", 
                    dp->inode, dp->rec_len, dp->name_len, temp);
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }
}


void *printIPinfo(char fileName[], INODE *ip)
{
    int i = 0;
    //TODO: Print information out of current ip
    //! i_block[15] contains pointers to disck blocks of a file ref P305
    printf("\n%s fileSize = %d\n", fileName,ip->i_size);
    
    printf("**i_block information***\n");
    i=0;
    while(i < 15)
    {
        printf("i_block[%d] = %d\n", i, ip->i_block[i]);
        i++;
    }
}


INODE *path2inode(INODE *ip, char pathName[], int inodes_start)
{
    int i = 0;
    char *tokenizedPath[64];
    int numOfComponents=0;
    
    if(pathName != 0) //* If a pathname was passed
    {
        //* Tokenize path
        numOfComponents = tokenize(pathName, tokenizedPath);
        int j=0;
        
        //TODO:For debugging only
        printf("Tokenized path:> ");
        while(j < numOfComponents)
        {
            printf("/%s", tokenizedPath[j++]);
            fflush(stdout);
        }
        puts("");
        //TODO: Search will go here --!NOTE: may need to do ip
        int ino, blk, offset;
        int n = numOfComponents; 
    
        for (i=0; i < n; i++)
        {
            ino = search(ip, tokenizedPath[i]);
        
            if (ino==0)
            {
                printf("can't find %s\n", tokenizedPath[i]); 
                exit(1);
            }
                // Mailman's algorithm: Convert (dev, ino) to INODE pointer
            blk    = (ino - 1) / 8 + inodes_start; 
            offset = (ino - 1) % 8;        
            get_block(dev, blk, buf);
            ip = (INODE *)buf + offset;   // ip -> new INODE
        }
        //*Print information out of current ip
        printIPinfo(tokenizedPath[i-1], ip);
        return ip;
    }
    else
    {
        show_dir(ip);
    }
}