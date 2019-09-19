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

//char command[16];       //? Command string
//char cwd[256];
//char buff[256] = { 0 };
//char *myargv[246] = { 0 };
//int myargc = 0;

int executeCommand(char buff[], char *env[]);
int pipeCheck(char buff[], char *env[]);

int main(int argc, char *argv[], char *env[])
{
    //! For date in shell
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);

    char buff[256] = { 0 };
    char parseBuff[256 ]= { 0 };
    char *myargv[246] = { 0 };
    char command[16] = { 0 };       //? Command string
    char cwd[256];
    int myargc = 0;

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
    char redirectName[64] = { 0 };
    char pipeBuff[128] = { 0 };
    int pipepid = 0;

    //! get the path
    const char *path = getenv("PATH");
    printf("PATH: %s\n\n", path);

    //! Get user input
    while(1)
    {
        if (getcwd(cwd, sizeof(cwd)) == 0)
        {
            printf("getcwd Failed!\n\n");
        }
        printf("[ %04d/%02d/%02d ] BERR Shell [ %s ]\n|-$ ", tm.tm_year+1900, tm.tm_mon, tm.tm_mday, cwd);
        fgets(buff, sizeof(buff), stdin);
        buff[strlen(buff)-1] = 0; // *Get rid of '\n'
        strcpy(parseBuff, buff);

        while(buff[i] != '\0')
        {
            if(buff[i] == ' ')
            { 
                argcounter++;
            }
            i++;
        }

        //! Tokenize command and parameters
        i=0;
        strcpy(command, strtok(parseBuff, " "));
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
            argcounter =0;
        }
        //! Check if command == exit
        else if (strcmp(command, "exit") == 0)
        {
            exit(0);
        }
        else
        {
            //! If is not cd or exit check for pipes
            //TODO: Check for pipes & run command
            pipeCheck(buff, env);
        }
    }
    return 0;
}

int executeCommand(char buff[], char *env[])
{
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
    char redirectName[64] = { 0 };
    char pipeBuff[128] = { 0 };
    int pipepid = 0;
    const char *path = getenv("PATH");
    char *myargv[246] = { 0 }; //! TODO: MAY NEED TO REPLACE this with one passed in?
    char command[16];       //? Command string

    //! (Count number of spaces, check for redirection and pipes
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
                //!Clear buff of the redirect
                buff[i] = 0;
                buff[i+1] = 0;
                buff[i+2] = 0;

                j=i+3;
                int k=0;
                //TODO: Do the string copy here & clean buff string
                while (buff[j] != '\0')
                {
                    redirectName[k++] = buff[j];
                    buff[j++] = 0; //!delete the rest of none command chars
                }
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
                //!Clear buff of the redirect
                buff[i] = 0; //! Gets rid of '>'
                buff[i+1] = 0; //!Gets rid of the space ' '
                j=i+2;
                int k=0;
                //TODO: Do the string copy here & clean buff string
                while (buff[j] != '\0')
                {
                    redirectName[k] = buff[j];
                    buff[j++] = 0; //!delete the rest of none command chars
                }    
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
                pipeBuff[k] = buff[j];
                buff[j++] = 0; //!delete the rest of none command chars
                k++;
            }
            //TODO: We could strtok until | for the buff until then
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
    strcpy(tempPath, pathNames[i]); //? First path dir
    i++;
    strcat(tempPath, "/");
    printf("command: %s   tempPath to command: %s\n", command,tempPath);

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
                close(0); //! Close file descriptor 1, stdin 
                open(redirectName, O_RDONLY);

            }
            else if (stdoutFlag > 0)
            {
                //TODO: Split command from i forward
                close(1); //! Close file descriptor 1, stdout
                open(redirectName, O_WRONLY | O_CREAT, 0644);
            }
            else if (stdoutAppen > 0)
            {
                //TODO: Split command from i forward
                close(1); //! Close file descriptor 1, stdout
                open(redirectName, O_RDWR | O_APPEND, 0644);
            }
            //!=============== END IO REDIRECTION ==========================


            fflush(stdout);

            strcat(tempPath, command); //! concat tempPath and command
            printf("Prior to execve tempPath %s\n", tempPath);
            r = execve(tempPath, myargv, env);
            //exit(1);
            strcpy(tempPath, pathNames[++i]);
            strcat(tempPath, "/");

            //i++;    
        }                
    }
    i=0; //* Reset counter
    //TODO: RESET Redirection
    pipeFlag = 0;
    stdinFlag = 0;
    stdoutAppen = 0;
    stdoutFlag = 0;
}

int pipeCheck(char buff[], char *env[])
{
    int pipeFlag = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    char nextBuff[256] = { 0 };

    //TODO: If pipe is found:
        //! Parse the first buff
        //! Create a pipe
        //!
    
    //! While the buff is not NULL
    while(buff[i] != '\0')
    {
        if(buff[i] == '|') //TODO: HERE I AM
        {
            pipeFlag = 1;
            //TODO: Copy rest of string to pass to the next pipe
            //TODO: break and create pipe, else run the exec function passing in original buff
            

            //!Clear buff of the pipe
            buff[i] = 0; //! Gets rid of '|'
            buff[i+1] = 0; //!Gets rid of the space ' '
            j=i+2;
            k=0;
            //TODO: Do the string copy here & clean buff string
            while (buff[j] != '\0')
            {
                nextBuff[k] = buff[j];
                buff[j++] = 0; //!delete the rest of none command chars
                k++;
            }
            //break;
            //TODO: We could strtok until | for the buff until then
        }
        i++;
    }

    if (pipeFlag != 0) //! Create pipe
    {
        int pd[2] = { 0 };
        int n=0;
        i=0;
        int pid = 0;
        int status = 0;

        //! Pipe creaiton using pipe()
        pipe(pd);
        printf("pd=[%d, %d]\n", pd[0], pd[1]);
        pid=fork();

        if (pid)
        {
            // fork a child to share the pipe
            printf("parent %d close pd[0]\n", getpid());
            close(pd[0]); //Writer close pd[]0
            close(1);
            // parent is setup as pipe WRITER
            dup(pd[1]);
            close(pd[1]);

            //TODO: Execute using current cleaned up buff / "command"
            executeCommand(buff, env);

            //TODO: wait??
            pid=wait(&status);

            printf("parent %d exit\n", getpid());
        }
        else
        {
            printf("child %d close pd[1]\n", getpid());
            close(pd[1]);
            // child as pipe READER
            close(0);
            dup(pd[0]);
            close(pd[0]);

            executeCommand(nextBuff, env);

            //exit(1);
        }
    }
    else
    {
        executeCommand(buff, env);
    }
}
