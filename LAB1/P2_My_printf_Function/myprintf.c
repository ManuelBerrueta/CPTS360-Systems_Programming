/** 
    Given: putchar(char c) of Linux, which prints a char.
    2-1. Write YOUR own prints(char *s) fucntion to print a string.
    Given: The following printu() function prints an unsignced integer. */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

typedef unsigned int u32;

char *ctable = "0123456789ABCDEF";
int BASE = 10;
int BASE_HEX = 16;
int BASE_OCT = 8;

//! Pointers
char *cp;
int *ip;

int rpu(u32 x)
{  
    char c;
    if (x){
        c = ctable[x % BASE];
        rpu(x / BASE);
        putchar(c);
    }
}

int printu(u32 x)
{
    //? If x==0 then run putchar('0'), else run rpu(x)
    (x==0)? putchar('0') : rpu(x);
    putchar(' ');
}

int printd(int x)
{
    (x==0)? putchar('0') : rpu(x);
    putchar(' ');
}

//!  ==== HEX ====
int hex(u32 x)
{  
    char c;
    if (x){
        c = ctable[x % BASE_HEX];
        hex(x / BASE_HEX);
        putchar(c);
    }
}

int printx(u32 x)
{
    (x==0)? putchar('0') : hex(x);
    putchar(' ');
}

//!  ==== Oct ====
int oct(u32 x)
{  
    char c;
    if (x){
        c = ctable[x % BASE_OCT];
        oct(x / BASE_OCT);
        putchar(c);
    }
}

int printo(u32 x)
{
    (x==0)? putchar('0') : oct(x);
    putchar(' ');
}

int prints(char* buf)
{
    int i=0;
    while (buf[i] != '\0')
    {
        putchar(buf[i]);
        i++;
    }
}

int myprintf(char *fmt, ...)
{
    int i = 0;
    int fmtSize = strlen(fmt);
    int negcheck=0;

    //cp = (int*)fmt; //!worked
    cp = fmt;  //* cp is used to traverse the format string
    ip = &fmt; //* ip = the address of fmt to traverse up the stack of the parameters passed
    
    //! Traversing up the stack to the parameters passed to myprintf
    char *temp = cp;

    while(i < fmtSize)
    {
        i++;
        if (*cp == '\n')
        {
            putchar('\n');
            putchar('\r');
            cp++;
        }
        else if( *cp != '%')
        {
            putchar(*cp);
            cp++;   
        }
        else
        {
            i++;
            cp++; //! Move to the next char after
            switch((int)*cp)
            {
            case 'c':   //? char
                putchar(*(++ip));; 
                putchar(' ');
                cp++;
                break;
            case 's':   //? string
                prints((char *)(*(++ip)));
                cp++;
                break;
            case 'u':   //? unsigned integer
                printu(*(++ip));
                cp++;
                break;
            case 'd':   //? integer
                negcheck = (*(++ip));
                if (negcheck < 0)
                {
                    putchar('-');
                    negcheck = negcheck * -1;
                }
                printd(negcheck);
                cp++;
                break;
            case 'o':   //? unsigned integer in OCT
                printo(*(++ip));
                cp++;
                break;
            case 'x':   //? unsigned integer in HEX
                printx(*(++ip));
                cp++;
                break;

            default:
                break;
            }
        }
    }
}
