#ifndef BROPDOXUTIL_HPP
#define BROPDOXUTIL_HPP

#define MAXNAME 255
#define MAXFILES 65536
#define PACKETSIZE 16384

#define PORT 4000

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>

enum class req {sync, send, receive};

typedef std::vector<char> con_buffer_t;

struct file_info {
    char name[MAXNAME];
    char extension[MAXNAME];
    char last_modified[MAXNAME];
    int size;
};

typedef struct {
    req req_type;
    char userid[MAXNAME];
    struct file_info file;
    unsigned int num_packets;
} handshake_t;

typedef struct {
    bool ok;
    unsigned int num_packets;
} ack_t;

typedef struct {
    unsigned int num;
    con_buffer_t data[PACKETSIZE];
} packet_t;


// struct client {
//     int devices[2];
//     char userid[MAXNAME];
//     struct file_info files[MAXFILES];
//     int logged_in;
// };

int init_unix_socket(struct sockaddr_un* sock, char const* path);

void convert_to_handshake(handshake_t* hand, con_buffer_t* buffer);

void convert_to_data(con_buffer_t* data, packet_t* packet);

#endif // BROPDOXUTIL_HPP