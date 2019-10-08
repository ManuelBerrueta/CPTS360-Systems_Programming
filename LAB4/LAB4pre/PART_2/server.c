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
#define SA struct sockaddr

char line[MAX];
int n;

int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in saddr, caddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&saddr, sizeof(saddr));

    // assign IP, PORT
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&saddr, sizeof(saddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(caddr);

    // Accept the data packet from client and verification

    while (1)
    {

        printf("server accepting connection\n");
        connfd = accept(sockfd, (SA *)&caddr, &len);
        if (connfd < 0)
        {
            printf("server acccept failed...\n");
            exit(0);
        }
        else
            printf("server acccept the client...\n");

        printf("server acccept a client connection\n");

        // Processing loop: newsock <----> client
        while (1)
        {
            n = read(connfd, line, MAX);
            if (n == 0)
            {
                printf("server: client died, server loops\n");
                close(connfd);
                break;
            }

            // show the line string
            printf("server: read  n=%d bytes; line=[%s]\n", n, line);

            strcat(line, " ECHO");

            // send the echo line to client
            n = write(connfd, line, MAX);

            printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
            printf("server: ready for next request\n");
        }
    }
}
