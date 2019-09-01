#include "bst.h"

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
        sscanf(line, “%s %s”, command, pathname);
        index = fidnCmd(command);
    }
}
