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


int Print_InOrder_Traversal(NODE* tree, FILE *outFile)
{
    if (tree == NULL)
    {
        // printf("\n");
        return;
    }
    else
    {
        if(strcmp(tree->name, "/") == 0)
        {
            printf("%c %s\n", tree->type, tree->name);
        }
        else
        {

            //TODO: do a possible recursive call until nullptr
            //TODO: to circulate the path
            //TODO: Possibly use pwd
        }
        
        Print_InOrder_Traversal(tree->childPtr, outFile);
        Print_InOrder_Traversal(tree->siblingPtr, outFile);
    }
}

//TODO: insert_sibling?


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
        p->childPtr=p->siblingPtr;//! Parent->childPtr points to sibling
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