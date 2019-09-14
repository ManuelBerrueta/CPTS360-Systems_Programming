/******************************************************************************
 **                Shell Simulator Programming Project - 3.11                **
 **                                 by                                       **
 **                           Manuel Berrueta                                **
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
char command[16];       //? Command string
char pathname[64];      //? Pathname string
char dname[64];         //? Directory string holder
char bname[64];         //? Basename string holder

int main(int argc, char *argv[], char *env[])
{
    int i=0;
    char buff[256] = "\n";
    
    while(i <= 2)
    {
        printf("| [Date] Berr Shell [cwd: s]\n-:> ");
        scanf("%s", buff);
        int pid = fork();
        printf("%d\n", pid);
        printf("getpid: %d\n",getpid());
        i++;
    }
    
    


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
