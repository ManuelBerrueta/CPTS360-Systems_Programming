#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

#define BLKSIZE 4096
#define MAX 10000
typedef struct
{
    char *name;
    char *value;
}ENTRY;

ENTRY entry[MAX];

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
    char tempBasename[64]= "";
    strcpy(tempBasename, fname);
    printf("%s", basename(fname)); // print file basename
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


main(int argc, char *argv[])
{
    int i, m, r;
    char cwd[128];

    m = getinputs();  // get user inputs name=value into entry[ ]
    getcwd(cwd, 128); // get CWD pathname

    printf("Content-type: text/html\n\n");
    printf("<p>pid=%d uid=%d cwd=%s\n", getpid(), getuid(), cwd);

    printf("<H1>Echo Your Inputs</H1>");
    printf("You submitted the following name/value pairs:<p>");

    //! Note: you must use the value for each portion of the input
    //! The name is just a place holder
    for (i = 0; i <= m; i++)
    {
        printf("%s = %s<p>", entry[i].name, entry[i].value);
    }
    printf("%s = %s<p>", entry[i].name, entry[i].value);
    printf("<p>");      
    
    /*****************************************************************
     Write YOUR C code here to processs the command
            mkdir dirname
            rmdir dirname
            rm    filename
            cat   filename
            cp    file1 file2
            ls    [dirname] <== ls CWD if no dirname
    *****************************************************************/
    i=0;
    if( strcmp(entry[i].value, "mkdir") == 0)
    {
        char *dirname = entry[i+1].value;
        r = mkdir(dirname, 0755);
    } 
    else if( strcmp(entry[i].value, "rmdir") == 0)
    {
        char *dirname = entry[i+1].value;
        r = rmdir(dirname);
    }
    else if( strcmp(entry[i].value, "rm") == 0)
    {
        char *file_name = entry[i+1].value;
        r = unlink(file_name);
    } 
    else if( strcmp(entry[i].value, "cat") == 0)
    {
        char *file_name = entry[i+1].value;
        
        if(file_name == 0)
        {
            puts("===> Did not pass in a file to cat!");
        }
        else
        {
            FILE *fp;
            int c;
            fp = fopen(file_name, "r");

            if (fp==0)
            {
                puts("===> FILE DNE or Not a file");
            }
            else
            {
                while((c = fgetc(fp)) != EOF)
                {
                    putchar(c);
                }
                printf("\n");
            }
        }
        
    }
    else if( strcmp(entry[i].value, "cp") == 0)
    {
        char *file_name_1 = entry[i+1].value;
        char *file_name_2 = entry[i+2].value;
        
        if ((file_name_1 == 0) || (file_name_2 == 0))
        {
            puts("Missing paramaters to copy files");
        }
        else
        {
            int fd, gd;
            char buff[4096];
            int n, total =0;
            fd = open(file_name_1, O_RDONLY);
            if(fd < 0)
            {
                printf("Could not open file %s\n", file_name_1);
            }
            else
            {
                gd = open(file_name_2, O_WRONLY | O_CREAT, 0644);
                if(gd < 0)
                {
                    printf("Could not open file %s\n", file_name_2);
                }
                else
                {
                    while(n=read(fd, buff, BLKSIZE))
                    {
                        write(gd, buff, n);
                        total+=n;
                    }
                    printf("total=%d\n", total);
                    close(fd);
                    close(gd);
                }
            }
            
        }
    }        
    else if( strcmp(entry[i].value, "ls") == 0)
    {
        char *file_name = entry[i+1].value;
        struct stat mystat, *sp = &mystat;
        int r;
        char filename[64], path[1024], cwd[256];
        strcpy(filename, "./");

        if(file_name != 0)
        {
            strcpy(filename, file_name);
            //printf("TEST %s", filename);
        }
        if (r = lstat(filename, sp) < 0)
        {
            //printf("no such file %s\n", filename);
            //exit(1);
            //strcpy(filename, "./");
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


    // create a FORM webpage for user to submit again
    printf("</title>");
    printf("</head>");
    printf("<body bgcolor=\"#FF0000\" link=\"#330033\" leftmargin=8 topmargin=8");
    printf("<p>------------------ DO IT AGAIN ----------------\n");     
    //printf("<FORM METHOD=\"POST\" ACTION=\"http://cs360.eecs.wsu.edu/~kcw/cgi-bin/mycgi.bin\">");     
    //------ NOTE : CHANGE ACTION to YOUR login name ----------------------------
    printf("<FORM METHOD=\"POST\" ACTION=\"http://cs360.eecs.wsu.edu/~berrueta/cgi-bin/mycgi.bin\">");      
    printf("Enter command : <INPUT NAME=\"command\"> <P>");
    printf("Enter filename1: <INPUT NAME=\"filename1\"> <P>");
    printf("Enter filename2: <INPUT NAME=\"filename2\"> <P>");
    printf("Submit command: <INPUT TYPE=\"submit\" VALUE=\"Click to Submit\"><P>");
    printf("</form>");
    printf("------------------------------------------------<p>");      
    printf("</body>");
    printf("</html>");
}
