#include "helper.h"

//************************************* ls *************************************
struct stat mystat, *sp;
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

int ls_file(char *fname, int sockbuff)
{
    struct stat fstat, *sp;
    int r, i, n;
    char ftime[64];
    char linkbuff[4096];
    char buff[4096];
    sp = &fstat;
    if ((r = lstat(fname, &fstat)) < 0)
    {
        printf("can’t stat %s\n", fname);
        exit(1);
    }
    if ((sp->st_mode & 0xF000) == 0x8000) // if (S_ISREG())
        sprintf(buff, "%c",'-');
    if ((sp->st_mode & 0xF000) == 0x4000) // if (S_ISDIR())
        sprintf(buff, "%c",'d');
    if ((sp->st_mode & 0xF000) == 0xA000) // if (S_ISLNK())
        sprintf(buff, "%c",'l');
    for (i = 8; i >= 0; i--)
    {
        if (sp->st_mode & (1 << i)) // print r|w|x
        {
            sprintf(buff, "%c", t1[i]);
        }
        else
        {
            sprintf(buff, "%c", t2[i]);
        }
        // or print -
    }

    sprintf(buff, "%4d ", sp->st_nlink);   // link count
    sprintf(buff, "%4d ", sp->st_gid);     // gid
    sprintf(buff, "%4d ", sp->st_uid);     // uid
    sprintf(buff, "%8d ", sp->st_size);    // file size
    fflush(stdout);

    // print time
    strcpy(ftime, ctime(&sp->st_ctime)); // print time in calendar form
    ftime[strlen(ftime) - 1] = 0;   // kill \n at end
    sprintf(buff, "%s ", ftime);
    // print name
    char* bname =  basename(fname);
    if(bname == 0)
    {
        bname = dirname(fname);
    }
    sprintf(buff, "%s", bname); // print file basename
    // print -> linkname if symbolic file
    if ((sp->st_mode & 0xF000) == 0xA000)
    {
       //TODO: Added this code, was not provided. Needs checking.
       // use readlink() to read linkname
        readlink(sp, buff, 40); 
        sprintf(buff, " -> %s", linkbuff); // print linked name
    }
    sprintf(buff, "\n");
    n = write(sockbuff, buff, 256);
}


int ls_dir(char *dname, int sockbuff) //! From 8.6.5
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
        ls_file(filename, sockbuff);
        fflush(stdout);
    }
    //n = write(client)
    puts("");
}
//************************************* ls *************************************

