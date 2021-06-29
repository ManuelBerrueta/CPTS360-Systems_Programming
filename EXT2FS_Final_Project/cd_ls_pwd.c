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

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

change_dir(char *pathname)
{
    printf("chage_dir()\n");
    if(pathname == 0)
    {
        running->cwd = root;
    }
    else
    {
        int curr_ino = getino(pathname);

        //TODO: Find how to handle '..'

        MINODE *mip = iget(dev, curr_ino);
        if(!S_ISDIR(mip->INODE.i_mode))
        {
            printf("mip is not a DIR\n");
        }
        iput(running->cwd);
        running->cwd = mip;
    }
}


void *show_dir(INODE *ip)
{
    //char sbuf[BLKSIZE], char temp[256];
    char temp[256];
    char ftime[64];
    DIR *dp;
    char *cp;
    int i;

    for (i = 0; i < 12; i++) //* assume DIR at most 12 direct blocks
    {
        if (ip->i_block[i] == 0)
        {
            break;
        }

        // YOU SHOULD print i_block[i] number here
        get_block(dev, ip->i_block[i], sbuf);
        dp = (DIR *)sbuf;
        cp = sbuf;

        //puts(" Inode |   |   File Size   |Size   | FName Size | File Name");
        //puts(" Inode |   |   File Size   |Size   | FName Size | File Name");

        while (cp < sbuf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;//*Temp name place holder
            MINODE *mip = iget(dev, dp->inode);

            //printf("%4d       %4d      %4d        %s\n",
            //       dp->inode, dp->rec_len, dp->name_len, temp);
            //TODO: getino to get information, then put ino
            /* printf("%4d       %s\n",
                   mip->INODE.i_size, temp); */


            if ((mip->INODE.i_mode & 0xF000) == 0x8000) // if (S_ISREG())
                printf("%c",'-');
            if ((mip->INODE.i_mode & 0xF000) == 0x4000) // if (S_ISDIR())
                printf("%c",'d');
            if ((mip->INODE.i_mode & 0xF000) == 0xA000) // if (S_ISLNK())
                printf("%c",'l');
            for (int j = 8; j >= 0; j--)
            {
                if (mip->INODE.i_mode & (1 << j)) // print r|w|x
                {
                    printf("%c", t1[j]);
                }
                else
                {
                    printf("%c", t2[j]);
                }
                // or print -
            }



            printf("%4d ", mip->INODE.i_links_count);   // link count
            printf("%4d ", mip->INODE.i_gid);     // gid
            printf("%4d ", mip->INODE.i_uid);     // uid
            fflush(stdout);

            // print time
            strcpy(ftime, ctime(&mip->INODE.i_ctime)); // print time in calendar form
            ftime[strlen(ftime) - 1] = 0;   // kill \n at end
            printf("%s ", ftime);
            printf("%8d ", mip->INODE.i_size);    // file size
            printf("%s\n", temp);
            
            iput(mip);
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
    printf("\n%s fileSize = %d\n", fileName, ip->i_size);

    printf("**i_block information***\n");
    i = 0;
    while (i < 15)
    {
        printf("i_block[%d] = %d\n", i, ip->i_block[i]);
        i++;
    }
}


INODE *path2inode(INODE *ip, char pathName[], int inodes_start)
{
    int i = 0;
    int numOfComponents = 0;

    if (pathName[0] != 0) //* If a pathname was passed
    {
        //* Tokenize path
        numOfComponents = tokenize(pathName);
        int j = 0;

        //TODO:For debugging only
        printf("Tokenized path:> ");
        while (j < numOfComponents)
        {
            printf("/%s", name[j++]);
            fflush(stdout);
        }
        puts("");
        //TODO: Search will go here --!NOTE: may need to do ip
        int ino, blk, offset;
        int n = numOfComponents;

        for (i = 0; i < n; i++)
        {
            ino = search(ip, name[i]);

            if (ino == 0)
            {
                printf("can't find %s\n", name[i]);
                exit(1);
            }
            // Mailman's algorithm: Convert (dev, ino) to INODE pointer
            blk = (ino - 1) / 8 + inodes_start;
            offset = (ino - 1) % 8;
            get_block(dev, blk, buf);
            ip = (INODE *)buf + offset; // ip -> new INODE
        }
        //*Print information out of current ip
        printIPinfo(name[i - 1], ip);
        show_dir(ip);
        return ip;
    }
    else
    {
        //show_dir(ip);
        show_dir(running->cwd);
    }
}


int list_file(char *pathname, int inode_start)
{
    printf("list_file()\n");

    root->INODE.i_block[12]; //This is how you access inodes!
    //show_dir(&(root->INODE));
    path2inode(&(root->INODE), pathname, inode_start);
}


rpwd(MINODE *wd)
{
    //char myname[256];
    if (wd == root)
    {
        return;
    }

    //*from i_block[0] of wd->INODE : get myino of.parentino of..pip = iget(dev, parentino);
    //*from pip->INODE.i_block[0] : get myname string as LOCAL

    char temp[256];
    DIR *dp;
    char *cp;
    int i;

    int childino = wd->ino;

    //! i_block[0] is especial, it contains the directory entries
    //! it will always include at least '.' and the parent dir ".."
    get_block(dev, wd->INODE.i_block[0], sbuf);
    dp = (DIR *)sbuf; //* Currently "."
    cp = sbuf;
    cp += dp->rec_len;
    dp = (DIR *)cp; //* Advance to ".."

    //* This dp->name just contains ".."
    MINODE *pip = iget(dev,dp->inode); //* Parent Inode Pointer
    
    get_block(dev,pip->INODE.i_block[0],sbuf);

    while (cp < sbuf + BLKSIZE)
    {
        
        if(childino == dp->inode)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            break;
        }
        
        //printf("%4d       %4d      %4d        %s\n",
                //dp->inode, dp->rec_len, dp->name_len, temp);
        cp += dp->rec_len;
        dp = (DIR *)cp;
        
    }

    rpwd(pip); // recursive call rpwd() with pip

    printf("/%s", temp);
}


int pwd(MINODE *wd)
{
    printf("pwd()\n");

    if (wd == root)
    {
        printf("cwd = / \n");
    }
    else rpwd(wd);
    puts("");
}
