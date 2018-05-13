#ifndef BROPDOXUTIL_HPP
#define BROPDOXUTIL_HPP

#define MAXNAME 255
#define MAXFILES 65536
#define PACKETSIZE 16384

#define PORT 4000

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

/******************************************************************************
 * Types
 */

enum class req {sync, send, receive};

typedef std::vector<unsigned char> data_buffer_t;

struct file_info {
    char name[MAXNAME];
    char extension[MAXNAME];
    char last_modified[MAXNAME];
    int size;
};

typedef struct {
    pthread_t handler_id;
    // Handler* handler
} user_id_t;

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

/******************************************************************************
 * Headers
 */

int init_unix_socket(struct sockaddr_un& sock, char const* path);

void convert_to_handshake(handshake_t& hand, data_buffer_t& buffer);

void convert_to_data(data_buffer_t& data, packet_t& packet);
void convert_to_data(data_buffer_t& data, handshake_t& hand);
void convert_to_data(data_buffer_t& data, ack_t& ack);

#endif // BROPDOXUTIL_HPP