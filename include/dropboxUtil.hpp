#ifndef DROPBOXUTIL_HPP
#define DROPBOXUTIL_HPP

#define MAXNAME 255
#define MAXFILES 65536
#define PACKETSIZE 16384
#define HANDSIZE 780

#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

enum class req {sync, send, receive};

struct file_info {
    char name[MAXNAME];
    char extension[MAXNAME];
    char last_modified[MAXNAME];
    int size;
};

typedef struct handshake {
    req req_type;
    int num_packets;
    struct file_info file;
} handshake_t;

typedef std::vector<char> con_buffer_t;

// struct client {
//     int devices[2];
//     char userid[MAXNAME];
//     struct file_info files[MAXFILES];
//     int logged_in;
// };

int init_unix_socket(struct sockaddr_un* sock, char* path);

void convert_to_handshake(handshake_t* hand, con_buffer_t* buffer);

#endif // DROPBOXUTIL_HPP