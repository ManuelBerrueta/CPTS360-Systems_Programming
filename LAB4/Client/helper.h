#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/types.h> 
#include <arpa/inet.h>


#define MAX   256
#define PORT 1234

int ls_file(char *fname);
int ls_dir(char *dname);