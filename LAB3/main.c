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

    int i=0;
    int j=0;
    int k=0;
    int argcounter=0;
    char tempArg[64] = "\0";
    char * pathNames[36] = { 0 };
    int stdinFlag = 0;
    int stdoutFlag = 0;
    int stdoutAppen = 0;
    int pipeFlag = 0;
    int pd[2] = { 0 };
    char redirectName[64] = { 0 };
    char pipeName[64] = { 0 };

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
            else if(buff[i] == '>' && buff[i+1] == '>') //! >> redirection check
            {
                if ( i != 0)
                {
                    stdoutAppen = i;                    
                }
            }
            else if( buff[i] == '>') //! > redirection check
            {
                if ( i != 0)
                {
                    stdoutFlag = i;
                    //!Clear buff of the redirect
                    buff[i] = 0; //! Gets rid of '>'
                    buff[i+1] = 0; //!Gets rid of the space ' '
                    j=i+2;
                    k=0;
                    //TODO: Do the string copy here & clean buff string
                    while (buff[j] != '\0')
                    {
                        redirectName[k] = buff[j];
                        buff[j++] = 0; //!delete the rest of none command chars
                        k++;
                    }
                }
                
            }
            else if( buff[i] == '<') //! < redirection check
            {
                if ( i != 0)
                {
                    stdinFlag = i; //! Can use i to know where to split the str
                }                
            }
            else if(buff[i] == '|') //TODO: HERE I AM
            {
                pipeFlag = 1;
                //!Clear buff of the pipe
                buff[i] = 0; //! Gets rid of '|'
                buff[i+1] = 0; //!Gets rid of the space ' '
                j=i+2;
                k=0;
                //TODO: Do the string copy here & clean buff string
                while (buff[j] != '\0')
                {
                    pipeName[k] = buff[j];
                    buff[j++] = 0; //!delete the rest of none command chars
                    k++;
                }
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

        //! Attempt running execve with appending a path each name

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
            printf("\n======> DEAD CHILD=%d, STATUS=0x%04x <======\n\n", pid, status);
        }
        else
        {
            printf("%s\n", tempPath);
            // getchar();
            printf("CHILD=%d STARTED | My PARENT=%d\n", getpid(), getppid());

            while (r == -1)
            {
                //!=========== IO REDIRECTION ==================================
                int fd=0;

                //TODO: Split the command after <, >, or >>
                if (stdinFlag > 0)
                {
                    //TODO: Split command from i forward
                    fd = open("append_stdout_test.txt", O_RDONLY);
                    close(0); //! Close file descriptor 1, stdin
                    dup(fd); //! Rplaces first NULL descriptor, 0 in this case  
                }
                else if (stdoutFlag > 0)
                {
                    //TODO: Split command from i forward
                    fd = open("stdout_redirect.txt", O_CREAT);
                    close(1); //! Close file descriptor 1, stdout
                    dup(fd); //! Rplaces first NULL descriptor, 1 in this case
                    //memset(myargv,0,sizeof(myargv));
                }
                else if (stdoutAppen > 0)
                {
                    //TODO: Split command from i forward
                    fd = open("append_stdout_redirect.txt", O_APPEND);
                    close(1); //! Close file descriptor 1, stdout
                    dup(fd); //! Rplaces first NULL descriptor, 1 in this case  
                }
                //!=============== END IO REDIRECTION ==========================

                
                
                
                
                
                strcat(tempPath, command); //! concat tempPath and command
                printf("Prior to execve tempPath %s\n", tempPath);
                r = execve(tempPath, myargv, env);
                strcpy(tempPath, pathNames[i]);
                strcat(tempPath, "/");
                i++;    
            }
        }
        i=0; //* Reset counter

        //TODO: RESET Redirection
        //fd =0;
        pipeFlag = 0;
        stdinFlag = 0;
        stdoutAppen = 0;
        stdoutFlag = 0;


        //int r = execvp(command, myargv);
        //memset(myargv,0,sizeof(myargv));
        //TODO: Uset getopt(myargc, myargv.)
    }
    return 0;
}