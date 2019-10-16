/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 **               LAB 4 - TCP Programming Project                           **
 **                              Server                                     **
 **                                by                                       **
 **                          Manuel Berrueta                                **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

#include "helper.h"


char buff[MAX];
int n;

int main(int argc, char *argv[]) 
{ 
    int server_fd, client_fd, len, r; 
    struct sockaddr_in server_address, client_address;
    char command[64], pathname[192];

    printf("1. create a TCP socket\n");
    server_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_fd < 0)
    { 
        printf("socket creation failed\n"); 
        exit(1); 
    }

    printf("2. fill in [localhost IP port=1234] as server address\n");
    bzero(&server_address, sizeof(server_address)); 
    server_address.sin_family = AF_INET; 
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_address.sin_port = htons(PORT); 

    printf("3. bind socket with server address\n");
    if ((bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address))) != 0)
    { 
        printf("socket bind failed\n"); 
        exit(2); 
    }

    printf("4. server listens\n");
    if ((listen(server_fd, 5)) != 0)
    { 
        printf("Listen failed\n"); 
        exit(3); 
    }

    len = sizeof(client_address);
    while(1)
    {
        printf("server accepting connection\n");
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &len); 
        if (client_fd < 0)
        { 
            printf("server acccept failed\n"); 
            exit(4); 
        }
        printf("server acccepted a client connection\n"); 

       // server Processing loop
        while(1)
        {
            printf("server: ready for next request\n");
            n = read(client_fd, buff, MAX);
            if (n==0)
            {
                printf("server: client died, server loops\n");
                close(client_fd);
                break;
            }

            //* show the buff contents received from client
            printf("server: read  n=%d bytes; buff=[%s]\n", n, buff);
            //strcat(buff, " ECHO");

            //!!!!!!!!!!!!!!!!! Command Handling Code !!!!!!!!!!!!!!!!!!!!!!!!!!

            //TODO: sscanf to separate command from filename
            bzero(pathname, 192);
            bzero(command, 64);
            sscanf(buff, "%s %s", command, pathname);
            
            if( strcmp(command, "quit") == 0 )
            {
                printf("Quitting...\n");
                sleep(3);
                exit(1);
            }
            else if( strcmp(command, "mkdir") == 0 )
            {
                char *dirname = pathname;
                r = mkdir(dirname, 0755);
                if (r == 0) //* mkdir returns 0 if successful
                {
                    strcat(buff, " = successful!");
                    n = write(client_fd, buff, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buff);
                }
                else
                {
                    strcat(buff, " = NOT successful!");
                    n = write(client_fd, buff, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buff);
                }
            }
            else if( strcmp(command, "rmdir") == 0 )
            {
                char *dirname = pathname;
                r = rmdir(dirname);
                if (r == 0) //* rmdir returns 0 if successful
                {
                    strcat(buff, " = successful!");
                    n = write(client_fd, buff, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buff);
                }
                else
                {
                    strcat(buff, " = NOT successful!");
                    n = write(client_fd, buff, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buff);
                }
            }
            else if( strcmp(command, "rm") == 0 )
            {
                char *file_name = pathname;
                r = unlink(file_name);
                if (r == 0) //* rmdir returns 0 if successful
                {
                    strcat(buff, " = successful!");
                    n = write(client_fd, buff, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buff);
                }
                else
                {
                    strcat(buff, " = NOT successful!");
                    n = write(client_fd, buff, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buff);
                }
            }
            else if( strcmp(command, "cd") == 0 )
            {
                /*if( strcmp(pathname, 0) == 0 )
                {
                    r = chdir("/home/");
                } */
                r = chdir(pathname);
                if (r == 0) //* rmdir returns 0 if successful
                {
                    strcat(buff, " = successful!");
                    n = write(client_fd, buff, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buff);
                }
                else
                {
                    strcat(buff, " = NOT successful!");
                    n = write(client_fd, buff, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buff);
                }
            }
            else if( strcmp(command, "pwd") == 0 )
            {
                getcwd(pathname, 256);
                strcat(buff," = ");
                strcat(buff, pathname);
                n = write(client_fd, buff, MAX);
                printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buff);
            }
            else if( strcmp(command, "get") == 0 )
            {
                int outFile = open(pathname, O_RDONLY);
                int i = 0;
                if (outFile < 0)
                {
                    puts("%s = Failed to open\n");
                    n = write(client_fd,0,MAX);
                }
                else
                {
                    struct stat fileInfo;
                    fstat(outFile, &fileInfo);//!Note use stat for filename
                    int filesize = fileInfo.st_size;
                    bzero(buff, MAX); 
                    sprintf(buff, "%d",filesize);
                    //lseek(outFile, 0, SEEK_END);

                    //lseek(outFile, 0L, SEEK_SET);
                    n = write(client_fd,buff, MAX); //*Send filesize

                    n = read(client_fd, buff, MAX); //* Receive confirmation of filesize
                    while ( n = read(outFile, buff, MAX) )
                    {
                        n = write(client_fd, buff, n);
                    }
                    //{
                    //}
                    printf("FILE=%s | SIZE=%d  Sent succesfully\n\n", pathname, filesize);
                    close(outFile);
                }
                
            }
            else if( strcmp(command, "put") == 0 )
            {
                //TODO: Get Size of file
                bzero(buff, MAX); 
                n = read(client_fd, buff, MAX);

                if ( n == 0)
                {
                    //? FILE FAIL
                    puts("FILE TRANSFER FAIL");
                }
                else
                {
                    int i = 0;
                    int fileSize = atoi(buff);

                    strcpy(buff, "Verified incoming filesize = ");
                    strcat(buff, buff);
                    n = write(client_fd, buff, MAX);


                    int inFile = open(pathname, O_WRONLY | O_CREAT, 0755);
                    while (i < fileSize)
                    {
                        n = read(client_fd, buff, MAX);
                        i += n; //i = nunmber of bites received
                        write(inFile, buff,n);
                    }
                    //* While received bytes less than filesize
                    //while( i < n)
                    //{

                    //i = read(fd, buff, fileSize); //!reading from buf, writing to file
                    //}
                    puts("SUCCESSFUL FILE TRANSER");
                    printf("FILE=%S  |  SIZE=%d\n\n", pathname, i);
                    close(inFile);
                }
            }
            else if( strcmp(command, "ls") == 0)
            {
                struct stat mystat, *sp = &mystat;
                int r;
                char filename[64], path[1024], cwd[256];
                strcpy(filename, "./");

                //TODO: pesudo code is:
                //!   write everything to a file, may be change file descriptor?
                //! send contents file size to client
                //! and pretty much like get, send file contents over
                //! Instead of saving contents of the file print them in the client
                unlink("templs");
                sleep(1);

                int tempFile = open("templs", O_WRONLY | O_CREAT, 0755);

                if(strcmp(pathname, "") != 0) 
                {
                    strcpy(filename, pathname);
                }
                if (r = lstat(filename, sp) < 0)
                {
                    printf("no such file %s\n", filename);
                    //exit(1);
                    //strcpy(filename, "./");
                }
                strcpy(path, filename);
                if (path[0] != '/') // Then filename is relative : get CWD path
                { 
                    getcwd(cwd, 256);
                    strcpy(path, cwd);
                    strcat(path, "/");
                    strcat(path, filename);
                }
                if (S_ISDIR(sp->st_mode))
                {
                    ls_dir(path, tempFile);
                }
                else
                {
                    ls_file(path, tempFile);
                }
                close(tempFile);
                //TODO: Send file over like you would get and then delete after

                //!!!!!!!!!!!!!!! SEND temp ls OVER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


                int outFile = open("templs", O_RDONLY);
                int i = 0;
                if (outFile < 0)
                {
                    puts("%s = Failed to open\n");
                    n = write(client_fd,0,MAX);
                }
                else
                {
                    struct stat fileInfo;
                    fstat(outFile, &fileInfo);//!Note use stat for filename
                    int filesize = fileInfo.st_size;
                    bzero(buff, MAX); 
                    sprintf(buff, "%d",filesize);
                    //lseek(outFile, 0, SEEK_END);

                    //lseek(outFile, 0L, SEEK_SET);
                    n = write(client_fd,buff, MAX); //*Send filesize

                    n = read(client_fd, buff, MAX); //* Receive confirmation of filesize
                    while ( n = read(outFile, buff, MAX) )
                    {
                        n = write(client_fd, buff, n);
                    }                    
                    
                    
/*                     while ( n = read(outFile, buff, MAX) )
                    {
                        //sleep(1);
                        n = write(client_fd, buff, n);
                    }
                    n = write(client_fd,0,MAX);
                    n = write(client_fd,"-=-=-=END=-=-=-",MAX); */

                    printf("FILE=%s | SIZE=%d  Sent succesfully\n", pathname, filesize);
                    close(outFile);
                }
                //!!!!!!!!!!!!!!!! END send ls over !!!!!!!!!!!!!!!!!
            }
            else
            {
                //* send the echo buff to client 
                n = write(client_fd, buff, MAX);
                printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buff);
            }            
            //!!!!!!!!!!!!!!! End Command Handling Code !!!!!!!!!!!!!!!!!!!!!!!!
        }
    }
}
