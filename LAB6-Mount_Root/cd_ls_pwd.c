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
    int numOfComponents=0;
    
    if(pathName[0] != 0) //* If a pathname was passed
    {
        //* Tokenize path
        numOfComponents = tokenize(pathName);
        int j=0;
        
        //TODO:For debugging only
        printf("Tokenized path:> ");
        while(j < numOfComponents)
        {
            printf("/%s", gpath[j++]);
            fflush(stdout);
        }
        puts("");
        //TODO: Search will go here --!NOTE: may need to do ip
        int ino, blk, offset;
        int n = numOfComponents; 
    
        for (i=0; i < n; i++)
        {
            ino = search(ip, gpath[i]);
        
            if (ino==0)
            {
                printf("can't find %s\n", gpath[i]); 
                exit(1);
            }
                // Mailman's algorithm: Convert (dev, ino) to INODE pointer
            blk    = (ino - 1) / 8 + inodes_start; 
            offset = (ino - 1) % 8;        
            get_block(dev, blk, buf);
            ip = (INODE *)buf + offset;   // ip -> new INODE
        }
        //*Print information out of current ip
        printIPinfo(gpath[i-1], ip);
        return ip;
    }
    else
    {
        show_dir(ip);
    }
}

int list_file(char *pathname, int inode_start)
{
    printf("list_file(): under construction\n");

    root->INODE.i_block[12]; //This is how you access inodes!
    //show_dir(&(root->INODE));
    path2inode(&(root->INODE), pathname, inode_start);
}


int pwd(MINODE *wd)
{
    printf("pwd(): yet to be done by YOU\n");
}