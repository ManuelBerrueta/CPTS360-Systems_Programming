#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define BLKSIZE 4096
#define MAX 10000
typedef struct
{
    char *name;
    char *value;
}ENTRY;

ENTRY entry[MAX];

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
            }
        }
        
    }
    else if( strcmp(entry[i].value, "cp") == 0)
    {
        char *file_name_1 = entry[i+1].value;
        char *file_name_2 = entry[i+1].value;
        
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
                gd = open(file_name_2, O_WRONLY | O_CREAT);
                if(gd < 0)
                {
                    printf("Could not open file %s\n", file_name_2;
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
        if(file_name == 0)
        {
            //TODO: ls cwd    
        }
        else
        {
            //TODO: ls with file_name
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
