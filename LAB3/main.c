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
//! Print env vars
/*     while (env[i] != '\0')
    {
        printf("%s\n", env[i]);
        i++;
    } */

int main(int argc, char *argv[], char *env[])
{
    //! 3.11.1

    //! (1) Prompt user for input
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);

    int i=0;
    int argcounter=0;
    char tempArg[64] = "\0";
    char * pathNames[36] = { 0 };




    //! get the path
    const char *path = getenv("PATH");
    printf("PATH: %s\n", path);

    
    while(1)
    {
        if (getcwd(cwd, sizeof(cwd)) != 0)
        {

        }
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
            while(i <= argcounter)
            {
                myargv[i] = strtok(NULL, " ");
                i++;
            }
            i=0;
        }


        //! Check if command == cd
        if (strcmp(command, "cd") == 0)
        {
            if (argcounter != 0)
            {
                chdir(myargv[1]);
            }
            else
            {
                chdir(getenv("HOME"));
            }
        }

        //! Check if command == exit
        if (strcmp(command, "exit") == 0)
        {
            exit(0);
        }

        argcounter=0;
        //! Count Number Of Paths
        while(path[i] != '\0')
        {
            if(path[i] == ':')
            { 
                argcounter++;
            }
            i++;
        }
        argcounter++; //! for the last path without ":"
        i=0;

        //! Tokenize paths
        char commpath[64];
        pathNames[i] = strtok(path, ":");
        while (++i < argcounter)
        {
            pathNames[i] = strtok(NULL, ":");
        }
        i=0; //* Reset counter
        //! Code to append path to command:
        /* strcpy(commpath, tempPath);
        strcat(commpath, "/");
        strcat(commpath, command);
        int r = execve(commpath, myargv, *env);
        memset(tempPath,0,sizeof(tempPath));
        tempPath =strtok(NULL, ":");
        i++; */

        //! Attempt running execve with appending each name
        //TODO: use chdir of each execve in a loop until it works?

        char tempPath[64] = { 0 };
        strcpy(tempPath, pathNames[i]);
        i++;
        strcat(tempPath, "/");
        printf("command: %s   tempPath to command: %s\n", command,tempPath);
        //getchar();


        //! Modified using a loop inside the child process area

        strcat(tempPath, command); //! concat tempPath and command
        printf("command with Path: %s\n", tempPath);
        //getchar();

        int r = -1;
        int status;
        int pid = fork();
        if (pid)
        {
            printf("\n======> PARENT=%d WAITS for CHILD=%d to DIE <======\n", getpid(),pid);
            pid=wait(&status);
            printf("======> DEAD CHILD=%d, STATUS=0x%04x <======\n\n", pid, status);
        }
        else
        {
            printf("%s\n", tempPath);
            // getchar();
            printf("CHILD=%d STARTED | My PARENT=%d\n", getpid(), getppid());

            while (r == -1)
            {
                strcat(tempPath, command); //! concat tempPath and command
                printf("Prior to execve tempPath %s\n", tempPath);

                r = execve(tempPath, myargv, env);

                //printf("CHILD=%d EXIT my PARENT=%d\n", getpid(), getppid());

                strcpy(tempPath, pathNames[i]);
                strcat(tempPath, "/");
                i++;    
            }
            
        }
        i=0; //* Reset counter








        //! This kind of works but doesn't stop, it also it doesn't look like it passes the args?
        //! Also y
/*         while (i < argcounter && r == -1)
        {
            strcat(tempPath, command); //! concat tempPath and command
            printf("command with Path: %s\n", tempPath);
            //getchar();

            
            int pid = fork();
            if (pid)
            {
                pid=wait(&status);
            }
            else
            {
                printf("%s", tempPath);
                getchar();
                r = execve(tempPath, myargv, env);
                if (r != -1)
                {
                    return;
                }
            }
            i++;
            strcpy(tempPath, pathNames[i]);

            strcat(tempPath, "/");
        } */



        //! Working forking child process!
//        int r = -1;
/*         int status;
        int pid = fork();
        if (pid)
        {
            pid=wait(&status);
        }
        else
        {
            while (i < argcounter)
            {
                strcpy(tempPath, pathNames[i]);
                strcat(tempPath, "/");
                strcat(tempPath, command);
                r = execve(tempPath, myargv, *env);
                i++;
            }
            
            
            //chdir(tempPath);
            //int r = execve(command, myargv, *env);
            //int r = execvp(command, myargv);
            //sleep(2);
        } */
        

        

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