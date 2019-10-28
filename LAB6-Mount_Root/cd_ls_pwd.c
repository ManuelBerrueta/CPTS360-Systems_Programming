/************* cd_ls_pwd.c file **************/

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC proc[NPROC], *running;
extern char gpath[256];
extern char *name[64];
extern int n;
extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
extern char line[256], cmd[32], pathname[256];

char buf[1024], dbuf[1024], sbuf[256], devName[256];
char indirect_blk_buff[1024], double_indirect_blk_buff[1024];

#define OWNER 000700
#define GROUP 000070
#define OTHER 000007

change_dir()
{
    printf("chage_dir(): to be constructed\n");
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



int list_file()
{
    printf("list_file(): under construction\n");

    root->INODE.i_block[12]; //This is how you access inodes!
    show_dir(&(root->INODE));

}

int pwd(MINODE *wd)
{
    printf("pwd(): yet to be done by YOU\n");
}
