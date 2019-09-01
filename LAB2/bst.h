#ifndef BST_H
#define BST_H

#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>s

//!------------------------------  Globals ---------------------------------!\\   
NODE *root, *cwd;
char line[128];         //? User command line input
char command[16];       //? Command string
char pathname[64];      //? Pathname string
char dname[64];         //? Directory string holder
char bname[64];         //? Basename string holder


char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm",
               "reload", "save", "menu", "quit", NULL};

typedef struct node
{
    char name[64]; //? Name of the node
    char type;     //? 'D' = Directory, 'F' = File
    struct node *parentPtr;
    struct node *childPtr;
    struct node *siblingPtr;
}NODE;

#endif