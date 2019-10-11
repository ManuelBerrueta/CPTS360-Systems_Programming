/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 **               LAB 4 - TCP Programming Project                           **
 **                              Client                                     **
 **                                by                                       **
 **                          Manuel Berrueta                                **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>


#define MAX   256
#define PORT 1234

char buff[MAX], ans[MAX];
int n;

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
        printf("input a buff : ");
        bzero(buff, MAX);                // zero out buff[ ]
        fgets(buff, MAX, stdin);         // get a buff (end with \n) from stdin

        buff[strlen(buff)-1] = 0;        // kill \n at end
        if (buff[0]==0)                  // exit if NULL buff
        {
            exit(0);
        }

        // Send ENTIRE buff to server
        n = write(client_fd, buff, MAX);
        printf("client: wrote n=%d bytes; buff=(%s)\n", n, buff);

        // Read a buff from sock and show it
        n = read(client_fd, ans, MAX);
        printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
    }
} 