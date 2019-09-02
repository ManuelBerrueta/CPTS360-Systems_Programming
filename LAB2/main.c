/******************************************************************************
 **                                                                          **
 **     Unix/Linux File System Tree Simulator Programming Project - 2.13     **
 **                                                                          **
 ******************************************************************************/

//!                 Project Specification and Requirements
//* The project is to design and implement a C program to simulate 
//* the Unix/Linux file system tree. The program should work as follows.
//* (1). Start with a / node, which is also the Current Working Directory (CWD).
//* (2). Prompt the user for a command. Valid commands are:
//*        mkdir, rmdir, cd, ls, pwd, creat, rm, save, reload, menu, quit
//* (3). Execute the command, with appropriate tracing messages.
//* (4). Repeat (2) until the "quit" command, which terminates the program.

#include "filesystem.h"

char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm",
               "reload", "save", "menu", "quit", NULL};


int main()
{
    int index;
    char line[128], command[16], pathname[64];

    
    initialize(); //initialize root node of the file system tree
    
    while(1)
    {
        printf("input a commad line : ");
        fgets(line,128,stdin);
    
        //? "\n" is introduced by the return after the input
        //! Getting rid of the \n at the end of the string
        line[strlen(line)-1] = 0;
        
        //! Parsing: Seperating command from pathname
        sscanf(line, "%s %s", command, pathname);
        index = findCmd(command);
        int r = fptr[index](pathname); //TODO: Break this down

        if(r == 0)
        {
            break;
        }
    }
    return 0;
}
