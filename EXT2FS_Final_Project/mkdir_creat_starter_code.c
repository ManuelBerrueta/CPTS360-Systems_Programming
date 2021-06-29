#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <sys/stat.h>
#include <ext2fs/ext2_fs.h>

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR; // need this for new version of e2fs

#define BLKSIZE 1024
GD *gp;
SUPER *sp;
INODE *ip;
DIR *dp;

/********** globals *************/
int fd, dev;                         // dev = fd
int ninodes, nblocks;                // ninodes, nblocks numbers from SUPER
int bmap, imap, iblock, inode_start; // BMAP, IMAP, inodes start block numbers

char gpath[128]; // token strings
int n;           // number of token strings
char *name[64];  // pointers to token strings

char line[128], command[32], pathname[64];

int get_block(int fd, int blk, char buf[])
{
    lseek(fd, (long)blk * BLKSIZE, 0);
    read(fd, buf, BLKSIZE);
}

int put_block(int fd, int blk, char buf[])
{
    lseek(fd, (long)blk * BLKSIZE, 0);
    write(fd, buf, BLKSIZE);
}

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

// WRITE YOUR OWN int balloc(dev) function, which returns a FREE disk block number

char *disk = "mydisk";

int main(int argc, char *argv[])
{
    int i, ino;
    char buf[BLKSIZE];

    fd = dev = open(disk, O_RDWR);
    if (fd < 0)
    {
        printf("open %s failed\n", disk);
        exit(1);
    }

    // read SUPER block to verify it's an EXT2 FS
    get_block(fd, 1, buf);
    sp = (SUPER *)buf;
    // verfiy it's an EXT2 FS

    ninodes = sp->s_inodes_count;
    nblocks = sp->s_blocks_count;

    printf("ninodes = %d nblocks = %d\n", ninodes, nblocks);

    // read Group Descriptor 0 to get bmap, imap and iblock numbers
    get_block(fd, 2, buf);
    gp = (GD *)buf;

    bmap = gp->bg_block_bitmap;
    imap = gp->bg_inode_bitmap;
    iblock = gp->bg_inode_table;

    inode_start = iblock;
    printf("bmp=%d imap=%d iblock = %d\n", bmap, imap, iblock);
}
