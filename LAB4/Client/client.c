/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 **               LAB 4 - TCP Programming Project                           **
 **                              Client                                     **
 **                                by                                       **
 **                          Manuel Berrueta                                **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

#include "helper.h"

char buff[MAX], ans[MAX];
int n;

void *printMenu();

int main(int argc, char *argv[ ]) 
{ 
    int client_fd; 
    struct sockaddr_in server_address; 

    printf("1. create a TCP socket\n");
    client_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (client_fd < 0)
    { 
        printf("socket creation failed\n"); 
        exit(1); 
    }

    printf("2. fill in [localhost IP port=1234] as server address\n");
    bzero(&server_address, sizeof(server_address)); 
    server_address.sin_family = AF_INET; 
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    server_address.sin_port = htons(PORT); 

    printf("3. connect client socket to server\n");
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) != 0)
    { 
        printf("connection to server failed\n"); 
        exit(2);
    }
    
    printf("********  processing loop  *********\n");
    while(1)
    {
        printMenu();
        printf("input a buff : ");
        bzero(buff, MAX);                // zero out buff[ ]
        fgets(buff, MAX, stdin);         // get a buff (end with \n) from stdin

        buff[strlen(buff)-1] = 0;        // kill \n at end
        if (buff[0]==0)                  // exit if NULL buff
        {
            exit(0);
        }

        // Send ENTIRE buff to server
        //TODO: Move this possibly inside if( local commands then don't do this)
        if(buff[0] != 'l')
        {
            n = write(client_fd, buff, MAX);
        }
        
        //printf("client: wrote n=%d bytes; buff=(%s)\n", n, buff);

        //!!!!!!!!!!!! Interactive commands !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        char command[64], pathname[256];
        bzero(pathname, MAX);
        sscanf(buff, "%s %s", command, pathname);
        int fileSize, i;
        fileSize = i = 0;



        if( strcmp(command, "lmkdir") == 0 )
        {
            char *dirname = pathname;
            n = mkdir(dirname, 0755);
            if (n == 0) //* mkdir returns 0 if successful
            {
                printf("%s = Succesful\n\n", buff);
            }
            else
            {
                printf("%s = NOT Succesful\n\n", buff);
            }
        }
        else if( strcmp(command, "lrmdir") == 0 )
        {
            char *dirname = pathname;
            n = rmdir(dirname);
            if (n == 0) //* rmdir returns 0 if successful
            {
                printf("%s = Succesful\n\n", buff);
            }
            else
            {
                printf("%s = NOT Succesful\n\n", buff);
            }
        }
        else if( strcmp(command, "lrm") == 0 )
        {
            char *file_name = pathname;
            n = unlink(file_name);
            if (n == 0) //* unlink returns 0 if successful
            {
                printf("%s = Succesful\n\n", buff);
            }
            else
            {
                printf("%s = NOT Succesful\n\n", buff);
            }
        }
        else if( strcmp(command, "lcd") == 0 )
        {
            /*if( strcmp(pathname, 0) == 0 )
            {
                n = chdir("/home/");
            } */
            n = chdir(pathname);
            if (n == 0) //* cd returns 0 if successful
            {
                printf("%s = Succesful\n\n", buff);
            }
            else
            {
                printf("%s = NOT Succesful\n\n", buff);
            }
        }
        else if( strcmp(command, "lcat") == 0 )
        {
            //TODO: IMPLEMENT LCAT
            n = chdir(pathname);
            if (n == 0) //* cd returns 0 if successful
            {
                printf("%s = Succesful\n\n", buff);
            }
            else
            {
                printf("%s = NOT Succesful\n\n", buff);
            }
        }
        else if( strcmp(command, "lpwd") == 0 )
        {
            getcwd(pathname, 256);
            strcat(buff," = ");
            strcat(buff, pathname);
            printf("pwd = Succesful\n");
            printf("pwd = %s\n\n", buff);
        }
        else if( strcmp(command, "get") == 0 )
        {
            //TODO: Get Size of file
            bzero(ans, MAX); 
            n = read(client_fd, ans, MAX);
            
            if ( n == 0)
            {
                //? FILE FAIL
                puts("FILE TRANSFER FAIL");
            }
            else
            {
                fileSize = atoi(ans);

                strcpy(buff, "Verified incoming filesize = ");
                strcat(buff, ans);
                n = write(client_fd, buff, MAX);


                int inFile = open(pathname, O_WRONLY | O_CREAT, 0755);
                while (i < fileSize)
                {
                    n = read(client_fd, ans, MAX);
                    i += n; //i = nunmber of bites received
                    write(inFile, ans,n);
                }
                //* While received bytes less than filesize
                //while( i < n)
                //{

                //i = read(fd, ans, fileSize); //!reading from buf, writing to file
                //}
                puts("SUCCESSFUL FILE TRANSER");
                printf("FILE=%S  |  SIZE=%d\n\n", pathname, i);
                close(inFile);
            }
        }
        else if ( strcmp(command, "put")==0 )
        {
            //TODO: Get Size of file
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
                printf("FILE=%s | SIZE=%d  Sent succesfully", pathname, filesize);
                close(outFile);
            }
        }
        else if ( strcmp(command, "ls")==0 )
        {

        }
        else if ( strcmp(command, "lls")==0 )
        {
            //char *file_name = pathname;
            struct stat mystat, *sp = &mystat;
            int r;
            char filename[64], path[1024], cwd[256];
            strcpy(filename, "./");

            if(strcmp(pathname, "") != 0) //TODO: THIS LINE IS THE KILLER
            {
                strcpy(filename, pathname);
                //strcat(filename, "/./");
                //TODO: Might need to branch here using an else for the rest
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
                printf("file path before ls execution:\n %s\n", path);
                fflush(stdout);
            }
            if (S_ISDIR(sp->st_mode))
            {
                ls_dir(path);
            }
            else
            {
                ls_file(path);
            }
            bzero(pathname, MAX);
        }
        else  //* ECHO
        {
            n = read(client_fd, ans, MAX);
            printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
        }
        

        //!!!!!!!!!!! END Interactive Commands !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // Read a buff from sock and show it
        //n = read(client_fd, ans, MAX);
        //printf("\ncmd=%s\n%s\n\n",buff, ans);
        //printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);

    }
} 

void *printMenu()
{
    puts("** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **");
    puts("**  get  put   ls   cd   pwd   mkdir   rmdir   rm  **");
    puts("**      lcat  lls  lcd  lpwd  lmkdir  lrmdir  lrm  **");
    puts("** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **");
}
