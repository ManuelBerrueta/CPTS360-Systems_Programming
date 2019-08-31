#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>s

typedef struct node
{
    char name[64]; //? Name of the node
    char type;     //? 'D' = Directory, 'F' = File
    struct node *parentPtr;
    struct node *childPtr;
    struct node *siblingPtr;
}NODE;