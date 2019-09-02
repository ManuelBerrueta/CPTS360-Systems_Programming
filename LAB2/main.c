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

//!------------------------------  Globals ---------------------------------   
NODE *root;
NODE *cwd;
char line[128];         //? User command line input
char command[16];       //? Command string
char pathname[64];      //? Pathname string
char dname[64];         //? Directory string holder
char bname[64];         //? Basename string holder
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm", "reload", 
               "save", "menu", "quit", NULL};

int findCmd(char *command);
void initialize();
int dbname(char *pathname);
int tokenize(char *pathname);

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

/*         if( r == 0)
        {
            break;
        } */

    }
    return 0;
}

int findCmd(char *command)
{
    int i = 0;
    while(cmd[i])
    {
        if (!strcmp(command, cmd[i]))
        return i; // found command: return index i
        i++;
    }
    return -1; // not found: return -1
}

void initialize()
{
    printf("===> Initialized file system\n\n");
    //! sizeof(NODE): Returns the size of the struct in bytes,
    //!   by default memory is allocated in 4 byte increments.
    //!   Therefore anything allocated that is less than 4 bytes will be padded
    //! malloc: Allocates memory the size of NODE
    //! (NODE *) casts the newly allocated memory as a NODE type.
    root = (NODE *)malloc(sizeof(NODE));
    root->parentPtr = root;
    root->siblingPtr = root;
    root->type = 'D';
    //strcpy(root->type, "D"); //! Using '' passes as int, using "" passes char
    cwd = root;
}

int dbname(char *pathname)
{
    //! dirname() and basename() from libgen.h destroy the pathname!
    char temp[128];
    strcpy(temp, pathname); //* Make a copy of the path name
    strcpy(dname, dirname(temp));
    strcpy(temp, pathname);
    strcpy(bname, basename(temp));
}

int tokenize(char *pathname)
{
    char *s;
    s = strtok(pathname, "/");
    while (s)
    {
        printf("%s ", s);
        s = strtok(0, "/");
    }
}


//?============================== COMMANDS ====================================
int mkdir(char *pathname)
{
    NODE *p;
    NODE *q;
    NODE *start;

    printf("mkdir: name=%s\n", pathname);

    //TODO: Check pathname for the '/' at the end
    //TODO: strok
    
    if (strcmp(pathname, "/")==0)
    {
        printf("can't mkdir with /\n");
        return -1;
    }
    if (pathname[0]=='/')
    {
        start = root;
    }
    else
    {
        start = cwd;
    }
    printf("check whether %s already exists\n", pathname);
    p = search_child(start, pathname);
    if (p)
    {
        printf("name %s already exists, mkdir FAILED\n", pathname);
        return -1;
    }

    printf("ready to mkdir %s\n", pathname);
    q = (NODE *)malloc(sizeof(NODE));
    q->type = 'D';
    strcpy(q->name, pathname);
    insert_child(start, q);
    printf("mkdir %s OK\n", pathname);
    
    return 0;
}

int rmdir(char *pathname)
{
    printf("rmdir: %s\n", pathname);
}

int cd(char *pathname)
{
    printf("cd: %s\n", pathname);
}

int ls(char *pathname)
{
    NODE *p = cwd->childPtr;
    printf("cwd contents = ");
    while(p)
    {
        printf("[%c %s] ", p->type, p->name);
        p = p->siblingPtr;
    }
    printf("\n");
}

// NOTE: You MUST improve ls() to ls(char *pathname)

int pwd(char *pathname)
{
    printf("pwd: %s\n", pathname);
}

int creat(char *pathname)
{
    printf("creat: %s\n", pathname);
}

int rm(char *pathname)
{
    printf("rm: %s\n", pathname);
}

int save()
{
    puts("==> Saving File System");
    FILE *fp = fopen("mySavedFileSystem", "w+");
    fprintf(fp, "%c %s", 'D', "string\n");
    fclose(fp);
    puts("==> File System Saved Succesfuklly\n");
}

int reload(char *filename)
{
    initialize();
}

int menu(char *pathname)
{
    puts("Terminal menu - Select a Command from the following:\n");
}

int quit(char *pathname)
{
    puts("Goodbye\n\n");
    //save();
    exit(0);
    return 0;
}
