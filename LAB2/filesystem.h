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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>
#include <libgen.h>
#include <string.h>
#include <strings.h>

typedef struct node
{
    char name[64]; //? Name of the node
    char type;     //? 'D' = Directory, 'F' = File
    struct node *parentPtr;
    struct node *childPtr;
    struct node *siblingPtr;
}NODE;

NODE *new_node(char *name);
NODE *insert(NODE *node, char *name);

void initialize();

int mkdir(char *pathname);
int rmdir(char *pathname);
int cd(char *pathname);
int ls(char *pathname);
int pwd(char *pathname);
int creat(char *pathname);
int rm(char *pathname);
int save();
int reload(char *filename);
int menu(char *pathname);
int quit(char *pathname);

#endif
