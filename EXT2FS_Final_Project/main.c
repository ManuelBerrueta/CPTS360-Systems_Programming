/****************************************************************************
*                   KCW testing ext2 file system                            *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <sys/stat.h>
#include <ext2fs/ext2_fs.h>

#include "type.h"
#include "util.c"
#include "cd_ls_pwd.c"
#include "mkdir_creat.c"
#include "rmdir.c"
#include "link_unlink_symlink.c"

MINODE minode[NMINODE];
MINODE *root;
PROC proc[NPROC], *running;


char gpath[256]; // global for tokenized components
char *name[64];  // assume at most 64 components in pathname
int n;           // number of component strings

int fd, dev;
int nblocks, ninodes, bmap, imap, inode_start, iblock;
char line[256], cmd[32], pathname[256], dirname2[256];




int init()
{
    int i, j;
    MINODE *mip;
    PROC *p;

    printf("init()\n");

    for (i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];
        mip->dev = mip->ino = 0;
        mip->refCount = 0;
        mip->mounted = 0;
        mip->mptr = 0;
    }
    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];
        p->pid = i;
        p->uid = i;//changed this to i from 0
        p->cwd = 0;
        p->status = FREE;
        for (j = 0; j < NFD; j++)
            p->fd[j] = 0;
    }
}

// load root INODE and set root pointer to it
int mount_root()
{
    printf("mount_root()\n");
    root = iget(dev, 2);
}

char *disk = "mydisk";
int main(int argc, char *argv[])
{
    //* My Shell Stuff
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);

    
    
    int i, ino;
    char buf[BLKSIZE];
    if (argc > 1)
        disk = argv[1];

    printf("checking EXT2 FS ....");
    if ((fd = open(disk, O_RDWR)) < 0)
    {
        printf("\n-={ open %s FAILED }=-\n", disk);
        exit(1);
    }
    dev = fd;
    /********** read super block at 1024 ****************/
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;

    /* Verify it's an EXT2 file system *****************/
    if (sp->s_magic != 0xEF53)
    {
        printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
        exit(1);
    }
    printf("VERIFIED EXT2 File System = OK\n");
    ninodes = sp->s_inodes_count;
    nblocks = sp->s_blocks_count;
    printf("ninodes = %d nblocks = %d\n", ninodes, nblocks);

    // read Group Descriptor 0 to get bmap, imap and iblock numbers
    get_block(dev, 2, buf);
    gp = (GD *)buf;

    bmap = gp->bg_block_bitmap;
    imap = gp->bg_inode_bitmap;

    //* Both iblock and inode_start poin to begining of inodes
    //inode_start = gp->bg_inode_table;
    iblock = gp->bg_inode_table;
    inode_start = iblock;

    printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, inode_start);

    init();
    mount_root();

    printf("root refCount = %d\n", root->refCount);

    printf("creating P0 as running process\n");
    running = &proc[0];
    running->status = READY;
    running->cwd = iget(dev, 2);
    proc[1].cwd = iget(dev,2);

    printf("root refCount = %d\n", root->refCount);

    //printf("hit a key to continue : "); getchar();
    while (1)
    {
        //printf("[ %04d/%02d/%02d ] BERR Shell [ %s ]\n|-$ ", tm.tm_year+1900, tm.tm_mon, tm.tm_mday, cwd);
        printf("[ %04d/%02d/%02d ] BERR Shell [ cwd ]\n|-$ ", tm.tm_year+1900, tm.tm_mon, tm.tm_mday);
        
        printf("input command : [ls|cd|pwd|quit|mkdir|creat|rmdir]");
        fgets(line, 128, stdin);
        line[strlen(line) - 1] = 0;
        if (line[0] == 0)
            continue;
        pathname[0] = 0;
        bzero(pathname, 256);
        cmd[0] = 0;

        sscanf(line, "%s %s %s", cmd, pathname, dirname2);
        printf("cmd=%s pathname=%s  dirname2=%s\n", cmd, pathname, dirname2);

        //! This has to go in function pointers
        if (strcmp(cmd, "ls") == 0)
            list_file(pathname, inode_start);
        if (strcmp(cmd, "cd") == 0)
            change_dir(pathname);
        if (strcmp(cmd, "pwd") == 0)
            pwd(running->cwd);
        if (strcmp(cmd, "mkdir") == 0)
            make_dir();
        if (strcmp(cmd, "rmdir") == 0)
            rmdir();
        if (strcmp(cmd, "link") == 0)
            link();  
        if (strcmp(cmd, "creat") == 0)
            creat_file(); 
        if (strcmp(cmd, "quit") == 0)
            quit();
    }
}

int quit()
{
    int i;
    MINODE *mip;
    for (i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->refCount > 0)
            iput(mip);
    }
    exit(0);
}
