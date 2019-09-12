/******************************************************************************
 **                                                                          **
 **     Unix/Linux File System Tree Simulator Programming Project - 2.13     **
 **                                 by                                       **
 **                           Manuel Berrueta                                **
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
char temp_buffer[64];
char line[128];         //? User command line input
char command[16];       //? Command string
char pathname[64];      //? Pathname string
char dname[64];         //? Directory string holder
char bname[64];         //? Basename string holder
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm", "save", 
               "reload", "menu", "quit", NULL};

NODE *new_node(char *name);
NODE *search_child(NODE *parent, char *name);
int insert_child(NODE *parent, NODE *q);
int delete_child(NODE *parent, NODE *q);
int Print_InOrder_Traversal(NODE* tree, FILE *outFile, char *currpath);

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

//!!!!!!!!!!!!!!!!!!!!!! NODE Structs !!!!!!!!!!!!!!!!!!!!!!!!!!!

NODE *new_node(char *name)
{
    NODE *node = (NODE *)malloc(sizeof(NODE));
    strcpy(node->name, name); //strcpy copies the str including the null char
    node->childPtr = node->parentPtr = node->siblingPtr = 0;
    return node;
}

NODE *search_child(NODE *parent, char *name)
{
    //! Here you start with the child and then look at the siblings
    //! for a possible match
    
    NODE *p;
    printf("search for %s in parent DIR\n", name);
    p = parent->childPtr;
    if (p==0)
    {
        return 0;
    }
    while(p)
    {
        if (strcmp(p->name, name)==0)
        {
            return p;
        }
        
        p = p->siblingPtr;
    }
    return 0;
}

int insert_child(NODE *parent, NODE *q)
{
    NODE *p = parent->childPtr  ;
    printf("insert NODE %s into parent child list\n", q->name);
    if (p==0) //! Case when the parent has no chilidren
    {
        parent->childPtr = q; 
    }
    else //! Case where parent already had at least one child
    {
        while(p->siblingPtr) //! Iterate until p children pointer is null
        {
            p = p->siblingPtr;
        }
        p->siblingPtr = q;
    }
    q->parentPtr = parent;
    q->siblingPtr = 0;
}


int Print_InOrder_Traversal(NODE* tree, FILE *outFile, char *currpath)
{
    if (tree == NULL)
    {
        memset(currpath,0,sizeof(currpath));//!clear path buffer
        return;
    }
    else
    {
        if(strcmp(tree->name, "/") == 0)
        {
            fprintf(outFile, "%c %s\n", tree->type, tree->name);
        }
        else
        {
            fprintf(outFile, "%c %s/%s\n", tree->type, currpath, tree->name);
            strcat(currpath, "/");
            strcat(currpath, tree);
        }
        
        Print_InOrder_Traversal(tree->childPtr, outFile, currpath);
        Print_InOrder_Traversal(tree->siblingPtr, outFile, currpath);
    }
}


int delete_child(NODE *parent, NODE *q)
{
    NODE *p = parent->childPtr;
    NODE *temp;
    printf("Delete NODE %s from parent child list\n", q->name);
/*     if (p==0) //! Case when the parent has no chilidren
    {
        parent->childPtr = q; 
    } */

    //TODO: Theory if the parent is the node we are trying to delete
    if( strcmp(p->name, q->name) == 0)
    {
        //TODO: Must replace parent with a sibling
        temp = p;
        p->parentPtr->childPtr=p->siblingPtr;//! Parent->childPtr points to sibling
        //memset(temp,0,sizeof(temp));//!clear path buffer
        free(temp);
        return;
    }
    else
    {
        //! Iterate until p children pointer name is the node we are trying to delete
        while( strcmp(p->siblingPtr->name, q->name) )
        {
            p = p->siblingPtr;
        }
        temp = p->siblingPtr;

        //TODO: Check to see if the node we are going to delete has other siblings
        if(temp->siblingPtr != 0)
        {
            //! if it does have a sibling, attach it to the "parent sibling"
            p->siblingPtr = temp->siblingPtr;
            free(temp);
            temp=0;
        }
        else
        {
            free(p->siblingPtr); //! Frees the memory
            p->siblingPtr=0; //! Now points to null
        }
    }
    return;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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
    root->siblingPtr = 0;
    //root->siblingPtr = root;
    root->type = 'D';
    strcpy(root->name,"/");
    //strcpy(root->type, "D"); //! Using '' passes as int, using "" passes char
    cwd = root;
    pwd_traverse = (NODE *)malloc(sizeof(NODE));
    *pwd_traverse = *cwd;
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
        localPathname[strlen(localPathname)-1] = 0;  //! /get rid of slash       TODO:LI MIGHT NEED THIS IN CD
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

    //! EDGE CASES

    if(searchNode != 0)
    {
        searchNode = search_child(searchNode, bname );
        if(searchNode)
        {
            printf("-> %s already exists \n", bname);
            printf("mkdir %s FAIL\n", pathname);
            memset(pathname,0,sizeof(pathname));//!clear path buffer
            return -1;
        }
    }

    //! END NEW GOODIES

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
        printf("==> rmdir: %s SUCCESFUL\n", pathname);
        printf("==>> %s removed\n", bname);
        printf("--------------------------------------\n");
        memset(pathname,0,sizeof(pathname));
        return 0;
    }
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

    if (pathname[strlen(pathname)-1] == '/') 
    {
        localPathname[strlen(localPathname)-1] = 0;  //! /get rid of slash       TODO:LI MIGHT NEED THIS IN CD
    }

    //! Split pathname in to dname for dirs, and bname for base
    dbname(pathname);
    NODE *tempCWD = cwd;                                                        //TODO:
    char *tempPath = pathname;

    //! TESTING - REMOVE
    dbname(pathname); 
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
        *pwd_traverse = *cwd; //! Makes a copy of cwd 
        memset(pathname,0,sizeof(pathname)); //Clear pathname
        return 0;        
    }
    if ( strcmp(pathname, "../" ) == 0 )
    {
        cwd = cwd->parentPtr;
        *pwd_traverse = *cwd; //! Makes a copy of cwd 
        memset(pathname,0,sizeof(pathname)); //Clear pathname
        return 0;        
    }
    
    if ( strcmp(pathname, "/" ) == 0 )//TODO: This might need to be fixed
    {
        //tempCWD = root->childPtr;
        cwd = root;
        *pwd_traverse = *cwd; //! Makes a copy of cwd                           TODO: NEWCHANGE
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
    //! Here we should be at the parent directory
    //! Check if basename matches one of the childrenPtr of the parentPtr
    if (strcmp(bname, "..") == 0)
    {
        //TODO: Go back one directory
        //cwd = cwd->parentPtr;
        cwd = tempCWD->parentPtr;
        *pwd_traverse = *cwd; //! Makes a copy of cwd 
        printf("cd %s was succesful\n", pathname);
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
        *pwd_traverse = *cwd; //! Makes a copy of cwd                         //TODO: Watch here for issues
        printf("cd: %s was succesful\n", pathname);
        memset(pathname,0,sizeof(pathname));
        return 0;
    }
}

int ls(char *pathname)
{
    char localPathname[64] = {'\0'};
    strcat(localPathname, pathname);

    if (pathname[strlen(pathname)-1] == '/') 
    {
        localPathname[strlen(localPathname)-1] = 0;  //! /get rid of slash
    }

    dbname(pathname);
    NODE *tempCWD = (NODE *)malloc(sizeof(NODE));
    *tempCWD = *cwd;
    //! NODE *tempCWD  = cwd;  --Changed this due to bug
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
    //else if( *pathname == '/')
    else if( strcmp(pathname, "/") == 0 )
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
        return 0;
    }
    else
    {
        /* TODO:  
         *  else traverse through the path checking if each is a directory
         *  if it is a directory then assign tempCWD to it 
         *  if it fails check then return failed check
         *  else once we get to the actual path (bname)
         *  check once more for directory, if it is use tempcwd as a base and print
         * 
         *  then print its contents
         */
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
                            printf("==+> ls %s FAIL\n", pathname);
                        }
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
        //! Here we should be at the parent directory
        //! Check if basename matches one of the childrenPtr of the parentPtr
        if (strcmp(bname, "..") == 0)
        {
            //TODO: Go back one directory
            //!cwd = cwd->parentPtr; !cd only
            tempCWD = tempCWD->parentPtr;
            printf("ls %s:\n", pathname);

            printf("root contents = ");
            while(tempCWD)
            {
                printf("[%c %s] ", tempCWD->type, tempCWD->name);
                tempCWD = tempCWD->siblingPtr;
            }
            printf("\n");
            
            memset(pathname,0,sizeof(pathname)); //Clear pathname
            return 0;
        }
        tempCWD = search_child(tempCWD, bname);

        if ( tempCWD == 0 )
        {
            printf("==> %s does not exists\n", bname);
            printf("==+> ls %s FAILED\n", pathname);
            memset(pathname,0,sizeof(pathname));
            return -1;
        }
        if ( tempCWD->type != 'D' )
        {
            printf("%s of the given %s path is not a Directory", tempCWD->name, 
            pathname);
            printf("==+> ls %s FAILED\n", pathname);
            memset(pathname,0,sizeof(pathname));
            return -1;
        }
        else
        {
            tempCWD = tempCWD->childPtr;
            printf("ls %s:\n", pathname);

            printf("== Contents = ");
            while(tempCWD)
            {
                printf("[%c %s] ", tempCWD->type, tempCWD->name);
                tempCWD = tempCWD->siblingPtr;
            }
            printf("\n");

            printf("==> ls: %s was succesful\n", pathname);
            memset(pathname,0,sizeof(pathname));
            return 0;
        }
    }
}


int pwd()
{
    //TODO: while temp->name != "/"
    //! Logic:
    //! cwd is tracking of where we are in the file system at any momemnt in time
    //! pwd_traverse == global variable at current working directory
    //! We can use cwd->name as our starting point and save in a string
    //! Then we traverse backwards withe *temp_cwd or pwd_traverse
    //! i.e. pwd_traverse = cwd->parent
    //! Then we copy pwd_traverse->name to a string and concat the prior string
    //! including a "/" in between
    //! Do this until pwd_traverse == "/"
    //! Issues with pwd, may have to use cwd instead


    if (strcmp(pwd_traverse->name, "/") == 0)
    {
        if (strcmp(temp_buffer, "\0") == 0)
        {
            printf("pwd: /\n");
        }
        else
        {
            printf("pwd: %s\n", temp_buffer);
        }
        memset(pathname,0,sizeof(pathname));
        memset(temp_buffer,0,sizeof(temp_buffer));
        puts("==>PWD SUCCESS\n");
        //! Recopy current working directory to pwd_traverse
        pwd_traverse = 0;
        pwd_traverse = (NODE *)malloc(sizeof(NODE)); 
        *pwd_traverse = *cwd;
        // *pwd_traverse = *cwd changed to root, had to reallocate memory
        return 0;
    }
    memset(pathname,0,sizeof(pathname));
    strcat(pathname, temp_buffer);
    memset(temp_buffer,0,sizeof(temp_buffer));
    strcpy(temp_buffer, "/"); 
    strcat(temp_buffer, pwd_traverse->name); //? Copy name of cwd 
                                             //? should be "/cwd_name"
    strcat(temp_buffer, pathname);
   // strcpy(pathname, temp_buffer);
    pwd_traverse = pwd_traverse->parentPtr;    

    pwd();
}

int creat(char *pathname)
{
    NODE *searchNode;
    NODE *q;
    char localPathname[64] = {'\0'};
    strcat(localPathname, pathname);

    //! dbname breaks directory path to dname and new dirName in bname
    dbname(pathname); //This will separate the path and the basename

    printf("creat: name=%s\n", pathname);

    //! Check pathname for the '/' at the end of the name
    // ! Remove after the name of new dir
    if (pathname[strlen(pathname)-1] == '/') 
    {
        localPathname[strlen(localPathname)-1] = 0;  //! /get rid of slash       TODO:LI MIGHT NEED THIS IN CD
    }

    
    if(pathname == '\0' || strcmp(pathname, "") == 0)
    {
        puts("can't creat with no pathname provided");
        memset(pathname,0,sizeof(pathname));//!clear path buffer
        return -1;
    }
    if(pathname)
    if (strcmp(pathname, "/") == 0 || strcmp(pathname, ".") == 0 || strcmp(pathname, "..") == 0)
    {
        printf("can't creat with %s\n", pathname);
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
                    printf("creat %s FAIL", pathname);
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
                    printf("creat %s FAIL\n", pathname);
                    memset(pathname,0,sizeof(pathname));//!clear path buffer
                    return -1;

                }
        }
    }

        //! EDGE CASES

    if(searchNode != 0)
    {
        searchNode = search_child(searchNode, bname );
        if(searchNode)
        {
            printf("-> %s already exists \n", bname);
            printf("mkdir %s FAIL\n", pathname);
            memset(pathname,0,sizeof(pathname));//!clear path buffer
            return -1;
        }
    }

    //! END NEW GOODIES

    printf("--------------------------------------\n");
    printf("ready to creat %s\n", pathname);
    q = (NODE *)malloc(sizeof(NODE));
    q->type = 'F'; //! File type
    strcpy(q->name, bname);
    insert_child(start, q);
    printf("creat %s OK\n", pathname);
    printf("--------------------------------------\n");
    
    //! Set pathname to null
    memset(pathname,0,sizeof(pathname));

    return 0;
}

int rm(char *pathname)
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
        puts("==> FAIL: Can't rm with no pathname provided");
        memset(pathname,0,sizeof(pathname));//!clear path buffer
        return -1;
    }
    if(pathname)
    if (strcmp(pathname, "/") == 0 || strcmp(pathname, ".") == 0 || strcmp(pathname, "..") == 0)
    {
        printf("can't rm with %s\n", pathname);
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
                    printf("==>FAIL rm %s \n", pathname);
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
    
    //? We might need to search for the directory we are trying to delete still
    //? See if it exists...
    //? We could possibly use the loop above to do it all..?
    //? i counter must be at the top that way when we get to the bottom of
    //? the statement we can check
    //? If i == counter then search for bname


    //TODO: MUST Check if it the folder has children, if it does, don't delete!!

    printf("--------------------------------------\n");
    printf("ready to rm %s\n", pathname);

    //! Need to get rid of the node
    //! it will be a child, thus we need to make sure the parent still points
    //! to other children
    //! if the child is an end node, then we can simply just cut if off
    //! free the nodeand make sure the parent points to null

    //TODO: start should be parent node??? or is start the node we are trying to delete?


        //! Here we should be at the parent directory
    
    if (strcmp(bname, "..") == 0)
    {
        printf("==>FAIL rm %s\n", pathname);
        memset(pathname,0,sizeof(pathname)); //Clear pathname
        return 0;
    }
    //! Check if basename matches one of the childrenPtr of the parentPtr
    searchNode = search_child(searchNode, bname); //TODO: This now searches for the current dir we are triyng to delete!

    if ( searchNode == 0 )
    {
        printf("==> rm: %s FAILED\n", pathname);
        printf("==+> %s does not exist\n", bname);
        memset(pathname,0,sizeof(pathname));
        return -1;
    }
    if ( searchNode->type != 'F' )
    {
        printf("==> rm: %s FAILED\n", pathname);        
        printf("==+> %s of the given %s path is not a FILE\n", searchNode->name, 
        pathname);
        memset(pathname,0,sizeof(pathname));
        return -1;
    }
    else
    {
        //TODO: HERE WE DELETE
        //TODO: MUST Check if it the folder has children, if it does, don't delete!!
        
        //! Here searchnode is the file we are trying to delete
        //? Here we assign q as our temp node
        //? We check if the node we are trying to delete has children
        q=searchNode->childPtr;

        //? Should not happen since it is a file
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
            
            //TODO: Deal with siblings?


            //q = searchNode->parentPtr; //! assign q to be the temp parent pointer of node to delete
            //q->childPtr = 0; //!NULL the child off the parent node
            //free(searchNode); //! delete searchnode from memory
        }
        printf("==> r: %s SUCCESFUL\n", pathname);
        printf("==>> %s removed\n", bname);
        printf("--------------------------------------\n");
        memset(pathname,0,sizeof(pathname));
        return 0;
    }
    //! Set pathname to null
    memset(pathname,0,sizeof(pathname));

    return 0;
}

int save()
{
    puts("==> Saving File System");
    FILE *fp = fopen("mySavedFileSystem.txt", "w+");
    if (fp == 0)
    {
        puts("Error opening file for saving\n\n");
    }
    //fprintf(fp, "%c %s", 'D', "string\n");
    char tempPath[64] = "\0";
    //strcpy(tempPath, "/");
    Print_InOrder_Traversal(root, fp, tempPath);
    fclose(fp);
    puts("==> File System Saved Succesfully\n");
}

int reload(char *filename)
{
    char buff[255];
    char fileType[3];
    char tempPath[64];
    //initialize();
    puts("==> Loading File System");
    FILE *fp = fopen("mySavedFileSystem.txt", "r");
    //FILE *fp = fopen("pathSavedFileSystem.txt", "r");
    //fprintf(fp, "%c %s", 'D', "string\n");
    //while(get)
    //fscanf(fp, "%s", buff);

    //! Scan the first root but not mkdir
    // fscanf(fp, "%s", buff); //! Only gets until first white space
    fgets(buff, 64, fp);
    while(!feof(fp))
    {
        //fscanf(fp, "%s", buff); //! Only gets until first white space
        fgets(buff, 64, fp);
        //char *fileType = strtok(buff, "");
        //char *tempPath = strtok(NULL, "/");
        sscanf(buff, "%s %s", fileType, tempPath);
        if (strcmp(fileType, "D") == 0)
        {
            mkdir(tempPath);
        }
        else
        {
            creat(tempPath);
        }
        printf("%s\n", buff);
        memset(buff,0,sizeof(buff));//!clear path buffer
    }
    fclose(fp);
    puts("==> File System LOADED Succesfully\n");
}

int menu(char *pathname)
{
    puts("Terminal menu - Select a Command from the following:\n");
    puts("mkdir, rmdir, ls, cd, pwd, creat, rm, save,reload, menu, quit\n\n");
}

int quit(char *pathname)
{
    puts("Goodbye\n\n");
    puts("==> Saving file system . . . \n");
    save();
    puts("===> File System Saved\n");
    exit(0);
    return 0;
}