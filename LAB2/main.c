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
NODE *start;
NODE *pwd_traverse;
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
int pwd();
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
        memset(pathname,0,sizeof(pathname));//!clear path buffer
        if( r < 0)
        {
            puts("\nLast command was not succesful");
            puts("Check error output");
        }

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
    pwd_traverse = cwd;
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
        
        //TODO: Check if dir exist using search_child


        //TODO: return how deep it goes?
        //TODO: NEXT move up next dir with name of above
        s = strtok(0, "/");
    }
}


//?============================== COMMANDS ====================================

int mkdir(char *pathname)
{
    NODE *searchNode;
    NODE *q;
    char *tempPath = pathname;

    printf("mkdir: name=%s\n", pathname);

    //TODO: Check pathname for the '/' at the end
    //TODO: strok
    //TODO: Fix mkdir  " " blank folder name

    if (strcmp(pathname, "/") == 0 || strcmp(pathname, ".") == 0 || strcmp(pathname, "..") == 0)
    {
        printf("can't mkdir with %s\n", pathname);
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

    searchNode = start;

    //TODO: Idea is break the path down each time and see if it exists
    //! dbname breaks directory path to dname and new dirName in bname
    dbname(tempPath); //This will separate the path and the basename

    //! Now we have dname and bname, we will iterate through dname
    //! Issue: What if it doesn't have a / TODO: FIX THIS
    //?      If name is found move to the next child
    //?      If name is not found, return FAIL

    //! Check to see if there is any '/' in the pathname
    int i=0;
    int pathCounter = 0;
    while(pathname[i] != '\0')
    {
        if(pathname[i] == '/')
        { 
            if(i == 0)
            {

            }
            else
            {
                pathCounter++;
            }
            
        }
        i++;
    }

    if (pathCounter > 0) //! Means there is at least one '/' in the path
    {
        tempPath = strtok(dname, "/");
        printf("Traversing through: %s/ ", tempPath);
        
        i=0; //reset counter
        while(i < pathCounter)
        {
            //! Check the name is within the child & siblings
            searchNode = search_child(searchNode, tempPath);
            if (searchNode) //* If the current part of the path exists then it fails
            {
                if(searchNode->type == 'D')
                {
                    printf("%s exists", tempPath);
                }
                else
                {
                    printf("%s exists but it's not a directory\n", tempPath);
                    printf("mkdir %s FAIL", pathname);
                }
                
/*                 if(searchNode->type == 'F')
                {
                    printf("%s is a file, not a DIR\n");
                }
                printf("name %s already exists, mkdir FAILED\n", pathname);
                return -1; */
            }
            else
            {
                printf("Path at %s does not exist", tempPath);
                return -1; //! Move to the next child
            }
            tempPath = strtok(tempPath, "/");
            i++;
        }
    }

    //! If we made it to here, then we are good

    start = searchNode;
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    printf("--------------------------------------\n");
    printf("ready to mkdir %s\n", pathname);
    q = (NODE *)malloc(sizeof(NODE));
    q->type = 'D';
    strcpy(q->name, bname);
    insert_child(start, q);
    printf("mkdir %s OK\n", pathname);
    printf("--------------------------------------\n");
    
    //! Set pathname to null
    memset(pathname,0,sizeof(pathname));

    return 0;
}




int rmdir(char *pathname)
{
    printf("rmdir: %s\n", pathname);
}

int cd(char *pathname)
{
    //? 1. Find pathname node;
    //? 2. Check if it;s a DIR
    //? 3. Change CWD to point at DIR

    //TODO: Add functionality to use ../ and if you see . ignore it
    
    NODE *tempCWD = cwd;
    char *tempPath = pathname;

    //! If pathname is '.' - Don't do anything
    if ( strcmp(pathname, "." ) == 0)
    {
        memset(pathname,0,sizeof(pathname)); //Clear pathname
        return 0;
    }
    if ( strcmp(pathname, ".." ) == 0 ) 
    {
        cwd = cwd->parentPtr;
        memset(pathname,0,sizeof(pathname)); //Clear pathname
        return 0;        
    }
    if ( strcmp(pathname, "../" ) == 0 )
    {
        cwd = cwd->parentPtr;
        memset(pathname,0,sizeof(pathname)); //Clear pathname
        return 0;        
    }
    
    if ( strcmp(pathname, "/" ) == 0 )//TODO: This might need to be fixed
    {
        //tempCWD = root->childPtr;
        cwd = root;
        memset(pathname,0,sizeof(pathname)); //Clear pathname
        return 0; 
    }
    
    dbname(pathname); //! Split pathname in to dname for dirs, and bname for base
    //! if next name is one of the sibling pointers && it's a dir
    //! then tempCWD point to this sibling

    //TODO: If we see a ".." we go back to tempCWD = tempCWD->parentPtr;
    //TODO: else we used search to see if the tempPath

    //! We count the number of '/' in the path, we will use this to iterate
    //! through the path
    int i=0;
    int pathCounter = 0;
    while(pathname[i] != '\0')
    {
        if(pathname[i] == '/')
        { 
            if(i == 0)
            {

            }
            else
            {
                pathCounter++;
            }
            
        }
        i++;
    }

    if (pathCounter > 0) //! Means there is at least one '/' in the path
    {
        tempPath = strtok(dname, "/");
        printf("Traversing through: %s/ ", tempPath);
        
        i=0; //reset counter
        while(i<(pathCounter-1))
        {
            if ( strcmp(tempPath, "..") == 0)
            {
                tempCWD = tempCWD->parentPtr;
            }
            else
            {
                //!Search for name
                //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! HERE WE ARE
                tempCWD = search_child(tempCWD, tempPath);
                if (tempCWD) //* If the current part of the path exists then it fails
                {
                    if(tempCWD->type == 'D')
                    {
                        printf("%s exists", tempPath);
                    }
                    else
                    {
                        printf("%s exists but it's not a directory\n", tempPath);
                        printf("cd %s FAIL", pathname);
                    }
                    
/*                     if(searchNode->type == 'F')
                    {
                        printf("%s is a file, not a DIR\n");
                    }
                    printf("name %s already exists, mkdir FAILED\n", pathname);
                    return -1; */
                }
                else
                {
                    printf("Path at %s does not exist", tempPath);
                    return -1; //! Move to the next child
                }
                tempPath = strtok(dname, "/");
                i++;
            }
        }
    }
/*     if ( (pathname+(strlen(pathname) -1)) == "/" )
    {
        char tempPath = strtok(dname, "/");
        printf("%s", "Traversing through: ");
    
        while (tempPath)
        {
            printf("%s", tempPath);
            printf("%s", "/");

            if ( tempPath == ".." )
            {
                tempCWD = cwd->parentPtr;
                
                return 0;        
            }
            else
            {
                //TODO: Check if current tokenized dir exists using search_child
                tempCWD = search_child(tempCWD, tempPath);
                //! If at anytime during this traversal p=0, then the target path
                //! does not exist
                if (tempCWD == 0)
                {
                    printf("%s of the given %s path does not exist", tempPath, 
                            pathname);
                    memset(pathname,0,sizeof(pathname));
                    return -1;
                }
                if ( tempCWD->type != 'D' )
                {
                    printf("%s of the given %s path is not a Directory", tempPath, 
                    pathname);
                    memset(pathname,0,sizeof(pathname));
                    return -1;
                }
                //TODO: Do we need to keep track how deep it goes?
                //TODO: NEXT move up next dir with name of above
                tempPath = strtok(0, "/");
            }
        }
    } */

    //! Here we should be at the parent directory
    //! Check if basename matches one of the childrenPtr of the parentPtr
    if (bname == "..")
    {
        memset(pathname,0,sizeof(pathname)); //Clear pathname
        return 0;
    }
    tempCWD = search_child(tempCWD, bname);

    if ( tempCWD == 0 )
    {
        printf("The given %s path does not exist", pathname);
        memset(pathname,0,sizeof(pathname));
        return -1;
    }
    if ( tempCWD->type != 'D' )
    {
        printf("%s of the given %s path is not a Directory", tempCWD->name, 
        pathname);
        memset(pathname,0,sizeof(pathname));
        return -1;
    }
    else
    {
        cwd = tempCWD;
        pwd_traverse = cwd;
        printf("cd: %s was succesful\n", pathname);
        memset(pathname,0,sizeof(pathname));
        return 0;
    }
}

int ls(char *pathname)
{


    //! If no string is passed after ls, then just list the CWD
    if ( *pathname == '\0')
    {
        NODE *p = cwd->childPtr;
        printf("cwd contents = ");

        while(p)
        {
            printf("[%c %s] ", p->type, p->name);
            p = p->siblingPtr;
        }
        printf("\n");
        memset(pathname,0,sizeof(pathname));
        return 0;
    }
    //! Else if the first char is /, then we start at root
    else if( *pathname == '/')
    {
        NODE *p = root->childPtr; //! *p = Traversing pointer
        
        //! This is the case to print root
        if ( strlen(pathname) == 1) 
        {       
            printf("root contents = ");
            while(p)
            {
                printf("[%c %s] ", p->type, p->name);
                p = p->siblingPtr;
            }
            printf("\n");
            memset(pathname,0,sizeof(pathname));
            return 0;
        }
        
        //TODO: If pathname is / + other str
        //TODO: This will also be similar or the same for the case where
        //TODO:     it is just a folder bad using cwd as the start point
        dbname(pathname); //!separe dir path and base name


        char *tempPath;
        tempPath = strtok(dname, "/");
        printf("%s", "Traversing through: ");
        while (tempPath)
        {
            printf("%s", tempPath);
            printf("%s", "/");

            //TODO: Check if current tokenized dir exists using search_child
            p = search_child(p, tempPath);
            //! If at anytime during this traversal p=0, then the target path
            //! does not exist
            if (p == 0)
            {
                printf("%s of the given %s path does not exist", tempPath, 
                        pathname);
                memset(pathname,0,sizeof(pathname));
                return -1;
            }
            if ( p->type != 'D' )
            {
                printf("%s of the given %s path is not a Directory", tempPath, 
                pathname);
                memset(pathname,0,sizeof(pathname));
                return -1;
            }
            //TODO: Do we need to keep track how deep it goes?
            //TODO: NEXT move up next dir with name of above
            tempPath = strtok(0, "/");
        }
        //TODO: use basename
        p = search_child(p, bname);
        printf("Contents of dir %s: ", p->name);
        while(p != 0)
        {
            while(p)
            {
                printf("[%c %s] ", p->type, p->name);
                p = p->siblingPtr;
            }
        }
        printf("\n");
        memset(pathname,0,sizeof(pathname));
        return 0;
    }
    else //! Else... a string is passed
    {
        //TODO: NEED TO WORK ON THIS PART
        dbname(pathname);
        //! use dname to search for the file pathsearch for
        /* if 
        {
            
        } */

    }
    
    *bname='\0';
    *dname='\0';
    memset(pathname,0,sizeof(pathname));
}

// NOTE: You MUST improve ls() to ls(char *pathname)

int pwd()
{
    //TODO: while temp->name != "/"
    char temp[128];
    if (pwd_traverse->name == "/")
    {
        printf("pwd: %s", pathname);
        memset(pathname,0,sizeof(pathname));
    }
    strcat(temp, "/"); //TODO: Possible issue with null characters
    strcat(temp, pwd_traverse->name);
    strcat(temp, pathname);
    strcpy(pathname, temp);
    pwd_traverse = pwd_traverse->parentPtr;    
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
