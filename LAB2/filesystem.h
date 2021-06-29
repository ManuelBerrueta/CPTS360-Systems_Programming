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
#include <stdlib.h>
//#include <fcntl.h>
#include <libgen.h>
#include <string.h>
#include <strings.h>

typedef struct node
{
    char name[64]; //? Name of the node
    char type;     //? 'D' = Directory, 'F' = File
    struct node *parentPtr;
    struct node *childPtr; //! Points to the oldest child
    struct node *siblingPtr; //! Points to the oldest sibling
}NODE;

/* NODE *new_node(char *name);
NODE *search_child(NODE *parent, char *name);
int insert_child(NODE *parent, NODE *q);
int delete_child(NODE *parent, NODE *q);
int Print_InOrder_Traversal(NODE* tree, FILE *outFile); */

#endif
