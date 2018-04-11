#define MAXNAME 255
#define MAXFILES 65536

#include <arpa/inet.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>

struct file_info {
    char name[MAXNAME];
    char extension[MAXNAME];
    char last_modified[MAXNAME];
    int size;
};

// struct client {
//     int devices[2];
//     char userid[MAXNAME];
//     struct file_info files[MAXFILES];
//     int logged_in;
// };

int init_unix_socket(sockaddr_un* sock, int type);