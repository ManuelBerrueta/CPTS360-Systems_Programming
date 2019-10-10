/* * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Code from 8.6.7 - The ls Program by K.C. Wang   *
 * from his book Systems Programming in Unix/Linux *
 * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

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
            printf("%c", t1[i]);
        else
            printf("%c", t2[i]);
        // or print -
    }

    printf("%4d ", sp->st_nlink);   // link count
    printf("%4d ", sp->st_gid);     // gid
    printf("%4d ", sp->st_uid);     // uid
    printf("%8d ", sp->st_size);    // file size
    
    // print time
    strcpy(ftime, ctime(&sp->st_ctime)); // print time in calendar form
    ftime[strlen(ftime) - 1] = 0;   // kill \n at end
    printf("%s ", ftime);
    // print name
    printf("%s", basename(fname)); // print file basename
    // print -> linkname if symbolic file
    if ((sp->st_mode & 0xF000) == 0xA000)
    {
        // use readlink() to read linkname
        readlink(sp, buff, 40); //TODO: This may be going wrong
        printf(" -> %s", buff); // print linked name
    }
    printf("\n");
}


int ls_dir(char *dname) //! From 8.6.5
{
    // use opendir(), readdir(); then call ls_file(name); man 3 opendir/readdir
    struct dirent *ep;
    DIR *dp = opendir(dname);
    while (ep = readdir(dp))
    {
        printf("name=%s ", ep->d_name);
    }
    ls_file(ep->d_name); //TODO: This may be going wrong
}


int main(int argc, char *argv[])
{
    struct stat mystat, *sp = &mystat;
    int r;
    char *filename, path[1024], cwd[256];
    filename = "./";
    // default to CWD
    if (argc > 1)
    {
        filename = argv[1]; // if specified a filename
    }
    if (r = lstat(filename, sp) < 0)
    {
        printf("no such file %s\n", filename);
        exit(1);
    }
    strcpy(path, filename);
    if (path[0] != '/') // Then filename is relative : get CWD path
    { 
        getcwd(cwd, 256);
        strcpy(path, cwd);
        strcat(path, "/");
        strcat(path, filename);
    }
    if (S_ISDIR(sp->st_mode))
    {
        ls_dir(path);
    }
    else
    {
        ls_file(path);
    }
}