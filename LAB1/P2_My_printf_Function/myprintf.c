#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

typedef unsigned int u32;

char *ctable = "0123456789ABCDEF";
int  BASE = 10; 

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

}

int printx(u32 x)
{
    
}

int printo(u32 x)
{
    
}



int myprintf(char *fmt, ...)
{
    while(fmt) //* While is not null
    {
        if(fmt != '%')
        {
            putchar(fmt++);
        }
        else
        {
            fmt++; //Move up to the next character after %

            switch ((int)fmt)
            {
            case 'c':   //? char
                putchar(fmt);
                break;
            case 's':   //? string
                /* code */
                break;
            case 'u':   //? unsigned integer
                printu(fmt);
                break;
            case 'd':   //? integer
                /* code */
                break;
            case 'o':   //? unsigned integer in OCT
                /* code */
                break;
            case 'x':   //? unsigned integer in HEX
                /* code */
                break;

            default:
                break;
            }
        }
        
        
    }
}

int main(int argc, char const *argv[])
{
/*     PART 2: myprintf Function
    Given: putchar(char c) of Linux, which prints a char.
    2-1. Write YOUR own prints(char *s) fucntion to print a string.
    Given: The following printu() function prints an unsigned integer. */

    return 0;
}
