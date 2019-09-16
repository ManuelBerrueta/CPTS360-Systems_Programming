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
char dname[64];         //? Directory string holder
char bname[64];         //? Basename string holder
char cwd[256];
char buff[256] = { 0 };
char *myargv[246] = { 0 };
int myargc = 0;

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
    int argcounter=0;
    char tempArg[64] = "\0";

    //! Print env vars
/*     while (env[i] != '\0')
    {
        printf("%s\n", env[i]);
        i++;
    } */

    //! get the path
    const char *path = getenv("PATH");
    printf("PATH: %s\n", path);

    
    while(1)
    {
        printf("[ %04d/%02d/%02d ] BERR Shell [ %s ]\n|-$ ", tm.tm_year+1900, tm.tm_mon, tm.tm_mday, cwd);
        fgets(buff, sizeof(buff), stdin);
        buff[strlen(buff)-1] = 0;

        //! (2) Handle simple commands
        while(buff[i] != '\0')
        {
            if(buff[i] == ' ')
            { 
                argcounter++;
            }
            i++;
        }

        i=0;
        //! Tokenize command and parameters
        strcpy(command, strtok(buff, " "));
        myargv[i] = command;
        i++;
        if(argcounter > 0)
        {
            while(i < argcounter)
            {
                myargv[i] = strtok(NULL, " ");
                i++;
            }
            i=0;
        }


        argcounter=0;
        //! Tokenize Paths
        while(path[i] != '\0')
        {
            if(path[i] == ':')
            { 
                argcounter++;
            }
            i++;
        }
        i=0;


        //! Working forking child process!
        int status;
        int pid = fork();
        if (pid)
        {
            pid=wait(&status);
        }
        else
        {
            //int r = execve(command, myargv, env);
            char *tempPath = strtok(path, ":");
            strcat(tempPath, command);
            while (execve(command, myargv, env) == -1)
            {
                tempPath = strtok(NULL, ":");
                strcat(tempPath, command);
            }
            //int r = execlp(command, myargv);
            sleep(2);
        }
        

        

        //memset(myargv,0,sizeof(myargv));
        
        //TODO: Parse buff strok " "
        //TODO: First arg will be the command
        //TODO: The rest will be parameters
        //TODO: Last paramemter will be the target
        //TODO: Count number of arguments
        //TODO: Uset getopt(myargc, myargv.)
    }

    //! PID Stuff
/*     int pid = fork();
    printf("pid=fork: %d\n", pid);
    printf("getpid: %d\n, getppid: %d\n",getpid()), getppid();
 */

    return 0;
}