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

    //cp = (int*)fmt; //!worked
    cp = fmt;
    ip = fmt;
    //printf("Address of cp: %8x", cp);
    
    //! Traversing up the stack to the parameters passed to myprintf
    char *temp = cp;
    int negcheck=0;


    while(i < fmtSize) //* While is not null
    {
        //if(fmt[i] != '%')
        //! Temp to check values
        temp = cp;

        if(*cp != '%')
        {
            //putchar(fmt[i++]);
            putchar(*cp);
            cp=cp++;
            //printf(fmt);
        }
        else
        {
            //i++; //Move up to the next character after %
            //! Decrenebt cp -2 up the stack to each parameter
            ip = ip-2;

            //switch ((int)fmt[++i])
            i++;
            // cp = cp+(i-1) ; //! Move to the next letter c,s,u,d,o - this worked
            cp = cp++; //! Move to the next letter c,s,u,d,o
            switch((int)*cp)
            {
            case 'c':   //? char
                putchar(*ip-1);
                putchar(' ');
                break;
            case 's':   //? string
                /* code */
                break;
            case 'u':   //? unsigned integer
                printu(*ip-1);
                break;
            case 'd':   //? integer
                temp = (*ip-1);
                break;
            case 'o':   //? unsigned integer in OCT
                printo(*ip-1);
                break;
            case 'x':   //? unsigned integer in HEX
                printx(*ip-1);
                break;

            default:
                break;
            }
        }
    }
    putchar('\n');
    putchar('\r');
}

int notprintf(char *fmt, ...)
{
    int i = 0;
    int fmtSize = strlen(fmt);
    int negcheck=0;

    //cp = (int*)fmt; //!worked
    cp = fmt;
    ip = &fmt; //* ip = the address of  fmt
    
    //! Traversing up the stack to the parameters passed to myprintf
    char *temp = cp;

    while(i < fmtSize)
    {
        i++;
        if (*cp == '\n')  //~ "\0"
        {
            putchar('\n');
            putchar('\r');
            cp++;
            //i++;
        }
        else if( *cp != '%')
        {
            putchar(*cp);
            putchar('\n'); //! GDB won't print without this
            //cp=cp+1; //? cp++ will not work....
            cp++;
            //i++;    
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
                /*TODO: Read that string until '\0' 
                */
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

int main(int argc, char *argv[], char*env[])
{
/*     PART 2: myprintf Function
    Given: putchar(char c) of Linux, which prints a char.
    2-1. Write YOUR own prints(char *s) fucntion to print a string.
    Given: The following printu() function prints an unsignced integer. */
    //printf("testing\n");
    //myprintf("T%xap", "1", "2", "3");

    //notprintf("Testing"); //!TESTER

    notprintf("%u%c%x%o%d%s\n", 214, 'a', 100, 16, -1, "hello");
    //! String test
    //notprintf("%s", "myStr");
    //notprintf("%s%d", "Hello you are the: ", -1);
    //myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n", 
	//         'A', "this is a test", 100,    100,   100,  -100); 

    // myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n", 
	//         'A', "this is a test", 100,    100,   100,  -100);
    return 0;
}
