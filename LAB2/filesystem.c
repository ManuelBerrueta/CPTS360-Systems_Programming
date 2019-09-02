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
    //! Here you start with the child and then look at the siblings
    //! for a possible match
    
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
