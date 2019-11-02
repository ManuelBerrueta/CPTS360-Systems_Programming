/*********** util.c file ****************/

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC proc[NPROC], *running;

extern char gpath[256];
extern char *name[64];
extern int n;

extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
//extern char line[256], cmd[32], pathname[256];
char sbuf[256];

int get_block(int dev, int blk, char *buf)
{
    lseek(dev, (long)blk * BLKSIZE, 0);
    read(dev, buf, BLKSIZE);
}

int put_block(int dev, int blk, char *buf)
{
    lseek(dev, (long)blk * BLKSIZE, 0);
    write(dev, buf, BLKSIZE);
}

int tokenize(char *pathname)
{
    // tokenize pathname in GLOBAL gpath[]; pointer by name[i]; n tokens
    int numOfComponents =0;
    strcpy(gpath, pathname);
    
    if(pathname == 0)
    {
        printf("NULL STRING - Can't Tokenize Empty String!\n\n");
        return -1;
    }

    if( strcmp(gpath, "/") == 0)
    {
        printf("Looking to LS root inode\n");
        name[numOfComponents++] = gpath;
        return;
    }

    char* tempPath;
    name[numOfComponents++] = strtok(pathname, "/");

    while((tempPath = strtok(NULL, "/")))
    {
        name[numOfComponents++] = tempPath;
    }

    return numOfComponents;
}

// return minode pointer to loaded INODE
MINODE *iget(int dev, int ino)
{
    int i;
    MINODE *mip;
    char buf[BLKSIZE];
    int blk, disp;
    INODE *ip;

    for (i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];

        if (mip->refCount && mip->dev == dev && mip->ino == ino)
        {
            mip->refCount++;
            printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
            return mip;
        }
    }

    for (i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->refCount == 0)
        {
            //printf("allocating NEW minode[%d] for [%d %d]\n", i, dev, ino);
            mip->refCount = 1;
            mip->dev = dev;
            mip->ino = ino;

            // get INODE of ino to buf
            blk = (ino - 1) / 8 + inode_start;
            disp = (ino - 1) % 8;

            //printf("iget: ino=%d blk=%d disp=%d\n", ino, blk, disp);

            get_block(dev, blk, buf);
            ip = (INODE *)buf + disp;
            // copy INODE to mp->INODE
            mip->INODE = *ip;

            return mip;
        }
    }
    printf("PANIC: no more free minodes\n");
    return 0;
}

int iput(MINODE *mip)
{
    int i, block, offset;
    char buf[BLKSIZE];
    INODE *ip;

    if (mip == 0)
        return;

    mip->refCount--;

    if (mip->refCount > 0)
        return;
    if (!mip->dirty)
        return;

    /* write back */
    printf("iput: dev=%d ino=%d\n", mip->dev, mip->ino);

    block = ((mip->ino - 1) / 8) + inode_start;
    offset = (mip->ino - 1) % 8;

    /* first get the block containing this inode */
    get_block(mip->dev, block, buf);

    ip = (INODE *)buf + offset;
    *ip = mip->INODE;

    put_block(mip->dev, block, buf);
}

int search(INODE *ip, char *name)
{
    // YOUR search() fucntion as in LAB 6
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

int getino(char *pathname)
{
    int i, ino, blk, disp;
    INODE *ip;
    MINODE *mip;

    printf("getino: pathname=%s\n", pathname);
    if (strcmp(pathname, "/") == 0)
        return 2;

    if (pathname[0] == '/')
        mip = iget(dev, 2);
    else
        mip = iget(running->cwd->dev, running->cwd->ino);

    n = tokenize(pathname);

    for (i = 0; i < n; i++)
    {
        printf("===========================================\n");
        ino = search(&(mip->INODE), name[i]);
        if (ino == 0)
        {
            iput(mip);
            printf("name %s does not exist\n", name[i]);
            return 0;
        }
        iput(mip);
        mip = iget(dev, ino);
    }
    iput(mip);
    return ino;
}
