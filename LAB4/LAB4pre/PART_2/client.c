#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define MAX 256
#define PORT 8080
//#define SA struct sockaddr

char line[MAX], ans[MAX];
int n;

int main()
{
    int sfd, cfd;
    struct sockaddr_in saddr;

    // socket create and varification
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&saddr, sizeof(saddr));

    // assign IP, PORT
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    saddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    printf("********  processing loop  *********\n");
    while (1)
    {
        printf("input a line : ");
        bzero(line, MAX);        // zero out line[ ]
        fgets(line, MAX, stdin); // get a line (end with \n) from stdin

        line[strlen(line) - 1] = 0; // kill \n at end
        if (line[0] == 0)           // exit if NULL line
            exit(0);

        // Send ENTIRE line to server
        n = write(sfd, line, MAX);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

        // Read a line from sock and show it
        n = read(sfd, ans, MAX);
        printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
    }
}
