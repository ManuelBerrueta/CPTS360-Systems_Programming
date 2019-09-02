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
#include <string.h>
#include <strings.h>

//!------------------------------  Globals ---------------------------------!\\   
NODE *root, *cwd;
char line[128];         //? User command line input
char command[16];       //? Command string
char pathname[64];      //? Pathname string
char dname[64];         //? Directory string holder
char bname[64];         //? Basename string holder

typedef struct node
{
    char name[64]; //? Name of the node
    char type;     //? 'D' = Directory, 'F' = File
    struct node *parentPtr;
    struct node *childPtr;
    struct node *siblingPtr;
}NODE;

void initialize();

int findCmd(char *command);

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

//TODO: Breakdown the table of function pointers
//? Table of function pointers
int (*fptr[])(char*)={ (int (*) ())mkdir, rmdir, ls, cd, pwd, creat, rm, save,
                                   reload, menu, quit};

#endif
