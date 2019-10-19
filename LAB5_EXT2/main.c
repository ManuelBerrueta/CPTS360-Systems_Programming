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
char buf[1024], dbuf[1024], sbuf[256];

int fd;
int bmap, imap, iblock;
int rootblock;

int get_block(int dev, int blk, char *buff);
void *dir(char *dev);


int main(int argc, char const *argv[])
{
    char dev[256] = { 0 };
    if (argc > 1)
    {
        *dev = argv[1];
    }
    else
    {
        *dev = "diskimage";
    }
    dir(dev);

    return 0;
}


// Reads a disk BLOCK into a char buff[BLKSIZE]
int get_block(int dev, int blk, char *buff)
{
    int n = -1;
    //* Move to an offset of block_number * BLKSIZE
    lseek(dev, blk * BLKSIZE, SEEK_SET);
    //* One at that offset read the block into the buffer
    n = read(dev, buff, BLKSIZE);

    if(n < 0)
    {
        printf("ERROR: Failed get_block %d\n", blk);
    }

    return n;
}


void *dir(char *dev)
{
    int i = 0;
    char *cp;

    //? Open diskimage for READ on
    fd = open(dev, O_RDONLY);
    if(fd < 0)
    {
        printf("FAILED to open %s\n", dev);
        exit(1);
    }
    // Read SUPER Block at given offset 
    int blockNum = 1;
    get_block(fd, blockNum, buf);

    //? (2) Read in SUPER block (block #1) to verify diskimage is an EXT2 FS
    sp = (SUPER *)buf; //* Casting the buffer as a SUPER Block
    //! Checking magic number to make it is a EXT2 File System:
    printf("Checking to make sure %s is a EXT2 File System", dev);
    if(sp->s_magic != 0xEF53)
    {
        printf("%s is NOT a EXT2 File System!\n",dev);
        exit;
    }
    else
    {
        printf("%s verified to be an EXT2 File System \n");
    }
    
    //?(3) Read Group Descriptor block in block #2
    get_block(fd, 2, buf);
    gp = (GD *) buf;

    printf("GD info:  %d %d %d %d %d %d\n",
            gp->bg_block_bitmap,
            gp->bg_inode_bitmap,
            gp->bg_inode_table,
            gp->bg_free_blocks_count,
            gp->bg_free_inodes_count,
            gp->bg_used_dirs_count);

    bmap = gp->bg_block_bitmap;
    imap = gp->bg_inode_bitmap;
    iblock = gp->bg_inode_table;
    printf("\nbmap = %d | imap = %d | iblock = %d\n", bmap, imap, iblock);

    //TODO: @pic 20191017_100943.jpg

    // Read first INODE block to get root inode #2
    //?(4) let INODE *ip
    get_block(fd, iblock, buf);
    ip = (INODE *)buf;
    ip++;

    printf("****** root inode info ******");
    printf("mode=%4x ", ip->i_mode);
    printf("uid=%d  |  gid=%d\n", ip->i_uid, ip->i_gid);
    printf("size=%d\n", ip->i_size);
    printf("time=%s", ctime(&ip->i_ctime));
    printf("link=%d\n", ip->i_links_count);
    printf("i_block[0]=%d\n", ip->i_block[0]);
    rootblock=ip->i_block[0];
    puts("*****************************");
    
    getchar(); //! breakpoint

    for (i=0; i < 2; i++)
    {
        if(ip->i_block[i] == 0)
        {
            break;
        }
    }

    get_block(fd, ip->i_block[i], dbuf);

    dp=(DIR *)dbuf;
    cp = dbuf;

    while(cp < dbuf + 1024)
    {
        strncpy(sbuf, dp->name, dp->name_len);
        sbuf[dp->name_len] = 0;
        printf("%4d %4d %4d %s\n");
        
    }

}
