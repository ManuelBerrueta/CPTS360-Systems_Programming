#include "helper.h"



//************************************* ls *************************************
struct stat mystat, *sp;
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

int ls_file(char *fname)
{
    struct stat fstat, *sp;
    int r, i;
    char ftime[64];
    char buff[4096];
    sp = &fstat;
    if ((r = lstat(fname, &fstat)) < 0)
    {
        printf("can’t stat %s\n", fname);
        exit(1);
    }
    if ((sp->st_mode & 0xF000) == 0x8000) // if (S_ISREG())
        printf("%c",'-');
    if ((sp->st_mode & 0xF000) == 0x4000) // if (S_ISDIR())
        printf("%c",'d');
    if ((sp->st_mode & 0xF000) == 0xA000) // if (S_ISLNK())
        printf("%c",'l');
    for (i = 8; i >= 0; i--)
    {
        if (sp->st_mode & (1 << i)) // print r|w|x
        {
            printf("%c", t1[i]);
        }
        else
        {
            printf("%c", t2[i]);
        }
        // or print -
    }

    printf("%4d ", sp->st_nlink);   // link count
    printf("%4d ", sp->st_gid);     // gid
    printf("%4d ", sp->st_uid);     // uid
    printf("%8d ", sp->st_size);    // file size
    fflush(stdout);

    // print time
    strcpy(ftime, ctime(&sp->st_ctime)); // print time in calendar form
    ftime[strlen(ftime) - 1] = 0;   // kill \n at end
    printf("%s ", ftime);
    // print name
    //char tempBasename[64]= "";
    //strcpy(tempBasename, fname);
    char* bname =  basename(fname);
    if(bname == 0)
    {
        bname = dirname(fname);
    }
    printf("%s", bname); // print file basename
    // print -> linkname if symbolic file
    if ((sp->st_mode & 0xF000) == 0xA000)
    {
       //TODO: Added this code, was not provided. Needs checking.
       // use readlink() to read linkname
        readlink(sp, buff, 40); 
        printf(" -> %s", buff); // print linked name
    }
    printf("\n");
}


int ls_dir(char *dname) //! From 8.6.5
{
    // use opendir(), readdir(); then call ls_file(name); man 3 opendir/readdir
    //* Added below code, was not provided as part of function

    struct dirent *ep;
    DIR *dp = opendir(dname);
    char filename[128];

    while (ep = readdir(dp))
    {
        //printf("name=%s ", ep->d_name);
        //TODO: 
        printf("%s ", ep->d_name);
        //memset(filename,0,128);
        //strcat(filename, "./");
        //strcat(filename, ep->d_name);
        //ls_file(dname);
        ls_file(dname);
        printf("<p>\n");
        fflush(stdout);
    }
    if(ep != 0)
    {
        
    }
}
//************************************* ls *************************************

