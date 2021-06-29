#include <stdio.h>
#include <stdlib.h>

int *FP;

int main(int argc, char *argv[ ], char *env[ ])
{
    int a,b,c;
    printf("enter main\n");

    printf("&argc=%x argv=%x env=%x\n", &argc, argv, env);
    printf("&a=%8x &b=%8x &c=%8x\n", &a, &b, &c);

    //! (1). Write C code to print values of argc and argv[] entries
    // TODO: TEST CODE

    printf("Value of argc is: %d\n", argc);

    int i=0;
    while(i < argc)
    {
        printf("The contents of the %d argument is: %s\n", i, argv[i]);
        i++;
    }
    //? The code Above is not tested ===================================

    a=1; b=2; c=3;
    A(a,b);
    printf("exit main\n");
}

int A(int x, int y)
{
    int d,e,f;
    printf("enter A\n");
    //! write C code to PRINT ADDRESS OF d, e, f
    // TODO: TEST CODE
    printf("Address of d: %x\n", &d);
    printf("Address of e: %x\n", &e);
    printf("Address of f: %x\n", &f);
    //? The code above is note tested ====================================

    d=4; e=5; f=6;
    B(d,e);
    printf("exit A\n");
}

int B(int x, int y)
{
    int g,h,i;
    printf("enter B\n");
    //! write C code to PRINT ADDRESS OF g,h,i
    // TODO: TEST CODE
    printf("Address of g: %x\n", &g);
    printf("Address of h: %x\n", &h);
    printf("Address of i: %x\n", &i);
    //? The code above is note tested ====================================

    g=7; h=8; i=9;
    C(g,h);
    printf("exit B\n");
}

int C(int x, int y)
{
    int u, v, w, i, *p; 
    printf("enter C\n");
    //! write C cdoe to PRINT ADDRESS OF u,v,w,i,p;
    // TODO: TEST CODE
    printf("Address of u: %x\n", &u);
    printf("Address of v: %x\n", &v);
    printf("Address of w: %x\n", &w);
    printf("Address of i: %x\n", &i);
    printf("Address of *p: %x\n", &p);

    u=10; v=11; w=12; i=13; 

//! (2). Write C code to print the stack frame link list.
    FP = (int *)getebp();  // FP = stack frame pointer of the C() function
    puts("\nPrinting stack frame list:");
    while(FP != 0)
    {
        if( *FP != 0)
        {
            printf("*FP=%x \t|\t FP=%x  \t|\n", *FP, FP);
        }
        else
        {
            printf("*FP=%x    \t|\t FP=%x  \t|\n", *FP, FP);
        }

        // TODO: Clean this up --> FP = FP - 4;
        FP = *FP;
    }

//! (3). Print the stack contents from p to the frame of main()
//!     YOU MAY JUST PRINT 128 entries of the stack contents.

    p = (int *)&p;
    puts("\nPrinting stack content:");
    i=0;
    while( i != 128)
    {
        printf("At i=%d  \t|\t ", i);
        printf("p=%8x  \t|\t*p=%8x\n", p, *p);
        p = (int *)&p +i++;
        //p = (int *)&p +(4*i++);
        //p = &p -(4*i++);65
    }

    printf("exit C\n");
}

//! (4). On a hard copy of the print out, identify the stack contents
//!      as LOCAL VARIABLES, PARAMETERS, stack frame pointer of each function.
