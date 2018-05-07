#ifndef BROPDOXUTIL_HPP
#define BROPDOXUTIL_HPP

#define MAXNAME 255
#define MAXFILES 65536
#define PACKETSIZE 16384

#define PORT 4000

#include "SocketHandler.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>

/******************************************************************************
 * Types
 */

enum class req {sync, send, receive};

typedef std::vector<char> data_buffer_t;

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
    data_buffer_t data[PACKETSIZE];
} packet_t;

typedef struct {
    Server* context;
    data_buffer_t* hand_package;
} arg_thread_t;

/******************************************************************************
 * Headers
 */

int init_unix_socket(struct sockaddr_un& sock, char const* path);

handshake_t* convert_to_handshake(data_buffer_t& data);
data_buffer_t* convert_to_data(packet_t& packet);
data_buffer_t* convert_to_data(handshake_t& hand);
data_buffer_t* convert_to_data(ack_t& ack);

#endif // BROPDOXUTIL_HPP