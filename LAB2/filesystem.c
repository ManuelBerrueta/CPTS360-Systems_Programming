#include "filesystem.h"

NODE *new_node(char *name)
{
    NODE *node = (NODE *)malloc(sizeof(NODE));
    strcpy(node->name, name); //strcpy copies the str including the null char
    node->childPtr = node->parentPtr = node->siblingPtr = 0;
    return node;
}

NODE *search_child(NODE *parent, char *name)
{
    NODE *p = parent->childPtr;
    printf("search for %s in parent DIR\n", name);
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
    if (p==0)
    {
        parent->childPtr = q;
    }
    else
    {
    while(p->siblingPtr)
    {
        p = p->siblingPtr;
    }
    p->siblingPtr = q;
    }
    q->parentPtr = parent;
    q->siblingPtr = 0;
}


//?============================== COMMANDS ====================================
int mkdir(char *pathname)
{
    printf("mkdir: %s\n", pathname);
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
    printf("ls: %s\n", pathname);
}

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
