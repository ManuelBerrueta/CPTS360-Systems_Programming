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

            sscanf(buff, "%s %s", command, pathname);
            
            if( strcmp(command, "mkdir") == 0 )
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
            else if( strcmp(command, "ls") == 0)
            {
                char *file_name = pathname;
                struct stat mystat, *sp = &mystat;
                int r;
                char filename[64], path[1024], cwd[256];
                strcpy(filename, "./");

                if(strcmp(file_name, "") == 0) //TODO: THIS LINE IS THE KILLER
                {
                    //strcpy(filename, ".");
                    getcwd(cwd, 256);
                    ls_dir(cwd);
                    //strcpy(filename, cwd);
                    //TODO: Might need to branch here using an else for the rest

                }
                else
                {
                    //TODO: Rest of the code
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
                    ls_dir(path);
                }
                else
                {
                    ls_file(path);
                }
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
