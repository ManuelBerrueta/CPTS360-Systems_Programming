#include "filesystem.h"

NODE *new_node(char *name)
{
    NODE *node = (NODE *)malloc(sizeof(NODE));
    strncpy(node->name, name, (strlen(name) + 1)); //TODO: TEST THIS!
    node->childPtr = node->parentPtr = node->siblingPtr = 0;
    return node;
}

void initialize()
{
    printf("Initialized file system\n");
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
    FILE *fp = fopen("mySavedFileSystem", "w+");
    fprintf(fp, "%c %s", 'D', "string\n");
    fclose(fp);
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
    return 0;
}
