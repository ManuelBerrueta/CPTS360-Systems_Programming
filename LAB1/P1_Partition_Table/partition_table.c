#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main()
{
    int fd;
    char buf[512];

    fd = open("vdisk", O_RDONLY);  // check fd value: -1 means open() failed
    read(fd, buf, 512);            // read sector 0 into buf[ ]

    lseek(fd, (long)123*512, SEEK_SET);  // lseek to byte 123*512 OR sector 123
    read(fd, buf, 512);                  // read sector 123 into buf[ ]

    return 0;
}