#include "filesystem.h"

NODE *new_node(char *name)
{
    NODE *node = (NODE *)malloc(sizeof(NODE));
    strcpy(node->name, name); //strcpy copies the str including the null char
    node->childPtr = node->parentPtr = node->siblingPtr = 0;
    return node;
}

NODE *insert(NODE *node, char *name)
{
    if ( node == 0 ) //! If node is empty
    {
        return new_node(name);
    }

    //TODO: if the file/directory already exist return printf("*Error: %s already exists \n", name);
    //TODO: may choose to share what kind of file it is ? dir or file?
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
