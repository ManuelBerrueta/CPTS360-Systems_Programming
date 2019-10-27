/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 **                  WSU CPTS 360-SYSTEMS PROGRAMMING                       **
 **                         LAB 6 - Mount Root                              **
 **                                by                                       **
 **                          Manuel Berrueta                                **
 **                                                                         **
 **     Summary: The Mount Root program does the following:                 **
 **                     1. Initialize FS data structures                    **
 **                     2. mount the root file system                       **
 **                     3. ls the root directory /                          **
 **                     4. cd and pwd                                       **
 **                                                                         **
 **     To build: cc mount_root.c -o mount_root -m32                        **
 **     Requires EXT2 File System libs: sudo apt install e2fslibs-dev       **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <ext2fs/ext2_fs.h>

char buff[1024], pathname[512], command[128];

#define TRUE 1
#define FALSE 0

int main(int argc, char const *argv[])
{
    while(TRUE)
    {
        bzero(buff, 1204);
        printf("%s", "Enter command followed by a space and a pathname::> ");
        fgets(buff, 1024, stdin);
        buff[strlen(buff)  -1] = 0;
        sscanf(buff,"%s %s", command, pathname);
        

        printf("buff: %s | command: %s | pathname: %s\n", buff, command, pathname);

        if(strcmp(command, "quit") == 0)
        {
            puts("Quitting..");
            exit(1);
        }
    }
    return 0;
}






//! NOTES:
/* 5. mount_root()  // mount root file system, establish / and CWDs
   {
      open device for RW (use the file descriptor as dev for the opened device)
      read SUPER block to verify it's an EXT2 FS
      record nblocks, ninodes as globals

      read GD0; record  bamp, imap, iblock as globals;
      
      root = iget(dev, 2);    // get root inode 
   
      Let cwd of both P0 and P1 point at the root minode (refCount=3)
          P0.cwd = iget(dev, 2); 
          P1.cwd = iget(dev, 2);

      Let running -> P0.
    } */