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

int delete_child(NODE *parent, NODE *q)
{
    NODE *p = parent->childPtr  ;
    printf("Delete NODE %s from parent child list\n", q->name);
/*     if (p==0) //! Case when the parent has no chilidren
    {
        parent->childPtr = q; 
    } */

    //! Iterate until p children pointer name is the node we are trying to delete
    while( strcmp(p->siblingPtr->name, q->name) )
    {
        p = p->siblingPtr;
    }

    free(p->siblingPtr); //! Frees the memory
    p->siblingPtr=0; //! Now points to null
}
