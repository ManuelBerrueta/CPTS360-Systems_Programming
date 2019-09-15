/******************************************************************************
 **                Shell Simulator Programming Project - 3.11                **
 **                                 by                                       **
 **                           Manuel Berrueta                                **
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

char command[16];       //? Command string
char pathname[64];      //? Pathname string
char dname[64];         //? Directory string holder
char bname[64];         //? Basename string holder
char cwd[256];
char buff[256] = "\0";

int main(int argc, char *argv[], char *env[])
{
    //! 3.11.1

    //! (1) Prompt user for input
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    if (getcwd(cwd, sizeof(cwd)) != 0)
    {
    }

    int i=0;
    
    while(i <= 2)
    {
        printf("[ %04d/%02d/%02d ] BERR Shell [ %s ]\n|-$ ", tm.tm_year+1900, tm.tm_mon, tm.tm_mday, cwd);
        scanf("%s", buff);

        i++;
    }

    //! (2) Handle simple commands

    //TODO: Maybe start a new process and pass on argv
    
    
    
    //! PID Stuff
/*     int pid = fork();
    printf("pid=fork: %d\n", pid);
    printf("getpid: %d\n, getppid: %d\n",getpid()), getppid();
 */

    //! Print Environment Vars
    printf("*env[]: ");
    while (env[i] != '\0')
    {
        printf("%s\n", env[i]); //! Prints all the Env Vars
        //if (strcmp(env[i], ))
        i++;
    }

    return 0;
}

int dbname(char *pathname)
{
    //! dirname() and basename() from libgen.h destroy the pathname!
    char temp[128];
    strcpy(temp, pathname); //* Make a copy of the path name
    strcpy(dname, dirname(temp));
    strcpy(temp, pathname);
    strcpy(bname, basename(temp));
}