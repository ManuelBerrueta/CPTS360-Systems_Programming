#include "helper.h"

//************************************* ls *************************************
struct stat mystat, *sp;
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

int ls_file(char *fname, int filebuff)
{
    struct stat fstat, *sp;
    int r, i, n;
    char ftime[64];
    char linkbuff[4096];
    char tempbuff[4096];
    char buff[4096];
    bzero(tempbuff, 4096);
    bzero(buff, 4096);
    sp = &fstat;
    if ((r = lstat(fname, &fstat)) < 0)
    {
        printf("can’t stat %s\n", fname);
        exit(1);
    }
    if ((sp->st_mode & 0xF000) == 0x8000) // if (S_ISREG())
    {
        sprintf(tempbuff, "%c",'-');
        strcat(buff, tempbuff);
    }
    if ((sp->st_mode & 0xF000) == 0x4000) // if (S_ISDIR())
    {
        sprintf(tempbuff, "%c",'d');
        strcat(buff, tempbuff);
    }
    if ((sp->st_mode & 0xF000) == 0xA000) // if (S_ISLNK())
    {
        sprintf(tempbuff, "%c",'l');
        strcat(buff, tempbuff);
    }
    for (i = 8; i >= 0; i--)
    {
        if (sp->st_mode & (1 << i)) // print r|w|x
        {
            sprintf(tempbuff, "%c", t1[i]);
            strcat(buff, tempbuff);
        }
        else
        {
            sprintf(tempbuff, "%c", t2[i]);
            strcat(buff, tempbuff);
        }
        // or print -
    }

    sprintf(tempbuff, "%4d ", sp->st_nlink);   // link count
    strcat(buff, tempbuff);
    sprintf(tempbuff, "%4d ", sp->st_gid);     // gid
    strcat(buff, tempbuff);
    sprintf(tempbuff, "%4d ", sp->st_uid);     // uid
    strcat(buff, tempbuff);
    sprintf(tempbuff, "%8d ", sp->st_size);    // file size
    strcat(buff, tempbuff);
    fflush(stdout);

    // print time
    strcpy(ftime, ctime(&sp->st_ctime)); // print time in calendar form
    ftime[strlen(ftime) - 1] = 0;   // kill \n at end
    sprintf(tempbuff, "%s ", ftime);
    strcat(buff, tempbuff);
    // print name
    char* bname =  basename(fname);
    if(bname == 0)
    {
        bname = dirname(fname);
    }
    sprintf(tempbuff, "%s", bname); // print file basename
    strcat(buff, tempbuff);
    // print -> linkname if symbolic file
    if ((sp->st_mode & 0xF000) == 0xA000)
    {
       //TODO: Added this code, was not provided. Needs checking.
       // use readlink() to read linkname
        readlink(sp, buff, 40); 
        sprintf(tempbuff, " -> %s", linkbuff); // print linked name
        strcat(buff, tempbuff);
    }
    sprintf(tempbuff, "\n");
    strcat(buff, tempbuff);
    n = write(filebuff, buff, 256);
}


int ls_dir(char *dname, int filebuff) //! From 8.6.5
{
    // use opendir(), readdir(); then call ls_file(name); man 3 opendir/readdir
    //* Added below code, was not provided as part of function

    struct dirent *ep;
    DIR *dp = opendir(dname);
    char filename[128];

    while (ep = readdir(dp))
    {
        //printf("name=%s ", ep->d_name);
        //ls_file(ep->d_name);
        strcpy(filename, dname);
        strcat(filename, "/");
        strcat(filename, ep->d_name);
        ls_file(filename, filebuff);
        fflush(stdout);
    }
    //n = write(client)
    puts("");
}
//************************************* ls *************************************

