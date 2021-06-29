//! To build this code:
//?      gcc -m32 tester.c myprintf.c -o myprintf

int main(int argc, char *argv[], char*env[])
{
    //!TESTER Code
    myprintf("argc=%x", argc);
 
    int i=0;
    myprintf("argv: ");
    while (i < argc)
    {
        myprintf(argv[i]);
        i++;
    }

    i=0;
    myprintf("*env[]: ");
    while (env[i] != '\0')
    {
        myprintf("%s\n", env[i]);
        i++;
    }
    

    myprintf("%u%c%x%o%d%s\n", 214, 'a', 100, 16, -1, "hello");

    myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n", 
	          'A', "this is a test", 100,    100,   100,  -100);
    


    return 0;
}