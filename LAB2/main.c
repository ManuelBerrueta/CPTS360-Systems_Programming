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
    strcpy(root->name,"/");
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



//?============================== COMMANDS ====================================

int mkdir(char *pathname)
{
    NODE *searchNode;
    NODE *q;
    char localPathname[64] = {'\0'};
    strcat(localPathname, pathname);

    //! dbname breaks directory path to dname and new dirName in bname
    dbname(pathname); //This will separate the path and the basename

    printf("mkdir: name=%s\n", pathname);

    //TODO: 
    // ! Possibly use if pathname[strlen(line)-1] == "/" then
    //!  { localPathname[strlen(line)-1] = 0} /get rid of slash
    //TODO:     It is possible if I used dname
    //! if dname == "." then use pathname alse you dname
    //TODO: deal with duplicates at root level
    //TODO: Fix mkdir  " " blank folder name

    //! Check pathname for the '/' at the end of the name
    // ! Remove after the name of new dir
    if (pathname[strlen(pathname)-1] == '/') 
    {
        localPathname[strlen(localPathname)-1] = 0;  //! /get rid of slash
    }

    
    if(pathname == '\0' || strcmp(pathname, "") == 0)
    {
        puts("can't mkdir with no pathname provided");
        memset(pathname,0,sizeof(pathname));//!clear path buffer
        return -1;
    }
    if(pathname)
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



    //! Now we have dname and bname, we will iterate through dname
    //! Issue: What if it doesn't have a / TODO: FIX THIS
    //?      If name is found move to the next child
    //?      If name is not found, return FAIL

    //! Check to see if there is any '/' in the pathname
    int i=0;
    int pathCounter = 0;
    while(localPathname[i] != '\0')
    {
        if(localPathname[i] == '/')
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
        char *tempPath = strtok(localPathname, "/");
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
                    printf("-> %s exists in the path\n", tempPath);
                }
                else
                {
                    printf("%s exists but it's not a directory\n", tempPath);
                    printf("mkdir %s FAIL", pathname);
                }
            }
            else
            {
                printf("Path at %s does not exist", tempPath);
                return -1; //! Move to the next child
            }
            tempPath = strtok(NULL, "/");
            i++;
            start = searchNode;
        }
    }
    else if( strcmp(dname, ".") == 0)
    {
        searchNode = search_child(searchNode, bname );
        if(searchNode)
        {
                if(searchNode->type == 'D')
                {
                    printf("-> %s already exists \n", bname);
                    printf("mkdir %s FAIL\n", pathname);
                    memset(pathname,0,sizeof(pathname));//!clear path buffer
                    return -1;

                }
        }
    }

    //! If we made it to here, then we are good

    //! - TESTING    start = searchNode;
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
    NODE *searchNode;
    NODE *q;
    char localPathname[64] = {'\0'};
    strcat(localPathname, pathname);

    //! dbname breaks directory path to dname and new dirName in bname
    dbname(pathname); //This will separate the path and the basename

    printf("rmdir: name=%s\n", pathname);

    //! Check pathname for the '/' at the end of the name
    // ! Remove after the name of new dir
    if (pathname[strlen(pathname)-1] == '/') 
    {
        localPathname[strlen(localPathname)-1] = 0;  //! /get rid of slash
    }
    
    if(pathname == '\0' || strcmp(pathname, "") == 0)
    {
        puts("==> FAIL: Can't rmdir with no pathname provided");
        memset(pathname,0,sizeof(pathname));//!clear path buffer
        return -1;
    }
    if(pathname)
    if (strcmp(pathname, "/") == 0 || strcmp(pathname, ".") == 0 || strcmp(pathname, "..") == 0)
    {
        printf("can't rmdir with %s\n", pathname);
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
    printf("==> Check whether %s already exists\n", pathname);

    searchNode = start;


    //! Check to see if there is any '/' in the pathname
    int i=0;
    int pathCounter = 0;
    while(localPathname[i] != '\0')
    {
        if(localPathname[i] == '/')
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
        char *tempPath = strtok(localPathname, "/");
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
                    printf("-> %s exists in the path\n", tempPath);
                }
                else
                {
                    printf("==>FAIL rmdir %s \n", pathname);
                    printf("%s exists but it's not a directory\n", tempPath);
                }
            }
            else
            {
                printf("Path at %s does not exist", tempPath);
                return -1; //! Move to the next child
            }
            tempPath = strtok(NULL, "/");
            i++;
            start = searchNode;
        }
    }
/*     else if( strcmp(dname, ".") == 0) //! Trying to delete a directory in cwd
    {
        searchNode = search_child(searchNode, bname );
        if(searchNode)
        {
                if(searchNode->type == 'D')
                {
                    printf("-> %s exists \n", bname);

                    //TODO Check to see if we this else if block
                    //! IF NOT DELETE


                    printf("rmdir %s SUCCESS", pathname);
                    memset(pathname,0,sizeof(pathname));//!clear path buffer
                    return -1;

                }
        }
    } */

    //! I BELIEVE IF WE MAKE IT HERE the path is correct
    //! If not we might have to do some rework...
    
    //? We might need to search for the directory we are trying to delete still
    //? See if it exists...
    //? We could possibly use the loop above to do it all..?
    //? i counter must be at the top that way when we get to the bottom of
    //? the statement we can check
    //? If i == counter then search for bname


    //TODO: MUST Check if it the folder has children, if it does, don't delete!!

    printf("--------------------------------------\n");
    printf("ready to rmdir %s\n", pathname);

    //! Need to get rid of the node
    //! it will be a child, thus we need to make sure the parent still points
    //! to other children
    //! if the child is an end node, then we can simply just cut if off
    //! free the nodeand make sure the parent points to null

    //TODO: start should be parent node??? or is start the node we are trying to delete?


        //! Here we should be at the parent directory
    
    if (strcmp(bname, "..") == 0)
    {
        printf("==>FAIL rmdir %s\n", pathname);
        memset(pathname,0,sizeof(pathname)); //Clear pathname
        return 0;
    }
    //! Check if basename matches one of the childrenPtr of the parentPtr
    searchNode = search_child(searchNode, bname); //TODO: This now searches for the current dir we are triyng to delete!

    if ( searchNode == 0 )
    {
        printf("==> rmdir: %s FAILED\n", pathname);
        printf("==+> %s does not exist\n", bname);
        memset(pathname,0,sizeof(pathname));
        return -1;
    }
    if ( searchNode->type != 'D' )
    {
        printf("==> rmdir: %s FAILED\n", pathname);        
        printf("==+> %s of the given %s path is not a Directory\n", searchNode->name, 
        pathname);
        memset(pathname,0,sizeof(pathname));
        return -1;
    }
    else
    {
        //TODO: HERE WE DELETE
        //TODO: MUST Check if it the folder has children, if it does, don't delete!!
        
        //! Here searchnode is the directory we are trying to delete
        //? Here we assign q as our temp node
        //? We check if the node we are trying to delete has children
        q=searchNode->childPtr;
        //! if i>0 then the node has children, we cannot delete
/*         i=0;
        while(q != 0)
        {
            q=q->childPtr;
            i++;
        } */
        //! Or just check if the childptr is not null
        if(q != 0)
        {
            printf("==> FAIL: Cannot delete %s\n", bname);
            printf("==+> %s is not empty!", bname);
            memset(pathname,0,sizeof(pathname));
            return -1;
        }
        else //!delete the node
        {
            delete_child(searchNode->parentPtr, searchNode);
            
            //TODO: Must delete from parents child list
            //q = searchNode->parentPtr; //! assign q to be the temp parent pointer of node to delete
            //q->childPtr = 0; //!NULL the child off the parent node
            //free(searchNode); //! delete searchnode from memory
        }
        



        printf("==> rmdir: %s was succesful\n", pathname);
        printf("==>> %s removed\n", bname);
        memset(pathname,0,sizeof(pathname));
        return 0;
    }


    //q = (NODE *)malloc(sizeof(NODE));
    //q->type = 'D';
    strcpy(q->name, bname);
    insert_child(start, q);


    printf("==> rmdir %s SUCCESFUL\n", pathname);
    printf("--------------------------------------\n");
    
    //! Set pathname to null
    memset(pathname,0,sizeof(pathname));

    return 0;
}


int cd(char *pathname)
{
    //? 1. Find pathname node;
    //? 2. Check if it's a DIR
    //? 3. Change CWD to point at DIR

    //TODO: Use mkdir to implement some of the full path

    char localPathname[64] = {'\0'};
    strcat(localPathname, pathname);

    //! Testing dname
    dbname(pathname);
    NODE *tempCWD = cwd;
    char *tempPath = pathname;


    //! TESTING - REMOVE
    dbname(pathname); //! Split pathname in to dname for dirs, and bname for base
    //! if next name is one of the sibling pointers && it's a dir
    //! then tempCWD point to this sibling

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

    //! Changing the starting point
    if (pathname[0]=='/')
    {
        tempCWD = root;
    }

    printf("check whether %s already exists\n", pathname);

    //! Need to fix this for the case of root "/"
    //searchNode = start;
    
    dbname(pathname); //! Split pathname in to dname for dirs, and bname for base
    //! if next name is one of the sibling pointers && it's a dir
    //! then tempCWD point to this sibling

    //TODO: else we used search to see if the tempPath

    //! We count the number of '/' in the path, we will use this to iterate
    //! through the path


    //! IMPORTANT NOTES:
    //! If i deal with dname and bname I am always dealing with the same cases
    //! Don't need to deal with the last slash and then bname is my target
    //! If i == pathcounter then we are at bname stage and its our final stop
    //! If we find bname exists then swap there and return

    int i=0;
    int pathCounter = 0;
    while(localPathname[i] != '\0') //! I could potentially use dname
    {
        if(localPathname[i] == '/')
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
        tempPath = strtok(localPathname, "/");
        printf("Traversing through: %s/ ", tempPath);
        
        i=0; //reset counter
        while(i<pathCounter)
        {
            if ( strcmp(tempPath, "..") == 0)
            {
                tempCWD = tempCWD->parentPtr;
                tempPath = strtok(NULL, "/");
                i++; //? NOT SURE
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
                        printf("%s exists\n", tempPath);
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
                //tempPath = strtok(dname, "/");
                tempPath = strtok(NULL, "/");
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
    if (strcmp(bname, "..") == 0)
    {
        //TODO: Go back one directory
        cwd = cwd->parentPtr;
        printf("cd %s was succesful", pathname);
        memset(pathname,0,sizeof(pathname)); //Clear pathname
        return 0;
    }
    tempCWD = search_child(tempCWD, bname);

    if ( tempCWD == 0 )
    {
        printf("==> %s does not exists\n", bname);
        printf("cd %s FAILED", pathname);
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
    char localPathname[64] = {'\0'};
    strcat(localPathname, pathname);
    dbname(pathname);
    NODE *tempCWD = cwd;
    char *tempPath = pathname;

    //! If no string is passed after ls, then just list the CWD
    if ( *pathname == '\0' || *pathname == '.')
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
    //! Else if pathname ==  '/', then we start at root
    else if( *pathname == '/')
    {
        NODE *p = root->childPtr; //! *p = Traversing pointer

        printf("root contents = ");
        while(p)
        {
            printf("[%c %s] ", p->type, p->name);
            p = p->siblingPtr;
        }
        printf("\n");
        memset(pathname,0,sizeof(pathname));
        return;
    }
    else
    {
        /* *  
         *  else traverse through the path checking if each is a directory
         *  if it is a directory then assign tempCWD to it 
         *  if it fails check then return failed check
         *  else once we get to the actual path (bname)
         *  check once more for directory, if it is use tempcwd as a base and print
         * 
         *  then print its contents
         */

    }
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
