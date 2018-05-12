#ifndef BROPBOXUTIL_HPP
#define BROPBOXUTIL_HPP

#define MAXNAME 255
#define MAXFILES 65536
#define PACKETSIZE 16384

#define TIMEOUT 200000

#define PORT 4000

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>


/******************************************************************************
 * Types
 */

enum class req {sync, send, receive};

typedef unsigned char data_buffer_t;

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
    unsigned int num_packets;
} ack_t;

typedef struct {
    unsigned int num;
    data_buffer_t data[PACKETSIZE];
} packet_t;

typedef struct {
    data_buffer_t* pointer;
    size_t size;
} convert_helper_t;

/******************************************************************************
 * Headers
 */

int init_unix_socket(struct sockaddr_un& sock, const char* path);

handshake_t* convert_to_handshake(data_buffer_t* data);
ack_t* convert_to_ack(data_buffer_t* data);
packet_t* convert_to_packet(data_buffer_t* data);
convert_helper_t convert_to_data(packet_t& packet);
convert_helper_t convert_to_data(packet_t const& packet);
convert_helper_t convert_to_data(handshake_t& hand);
convert_helper_t convert_to_data(ack_t& ack);

/******************************************************************************
 * Globals
 */

#endif // BROPBOXUTIL_HPP 