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

int dev;//! File Descriptor for Device
int bmap, imap, iblock;
int rootblock;

// Functions
int get_block(int dev, int blk, char *buff);
void *dir(char *devName);
void *show_dir(INODE *ip);


int main(int argc, char const *argv[])
{
    char ;
    if (argc > 1)
    {
        strcpy(devName, argv[1]);
    }
    else
    {
        strcpy(devName, "diskimage");
    }
    dir(devName);

    return 0;
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


void *dir(char *devName)
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

    //TODO: @pic 20191017_100943.jpg

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
    puts("*****************************************\n\n");
    
    //getchar(); //! breakpoint

    show_dir(ip);

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