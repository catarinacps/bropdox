#ifndef BROPBOXUTIL_HPP
#define BROPBOXUTIL_HPP

#define MAXNAME 255
#define MAXFILES 65536
#define PACKETSIZE 16384

#define MAX_FILE_LIST_SIZE 10

#define TIMEOUT 200000

#define ADDR "BropDoxServer"

#define PORT 4000

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
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
    char name[MAXNAME*2];
    char last_modified[MAXNAME];
    int size;

    bool operator < (file_info const& a) const
    {
        return name < a.name;
    }
};

typedef struct {
    file_info file_list[MAX_FILE_LIST_SIZE];
    bool has_next;
} file_info_list_t;

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
    unsigned int num_packets;
    size_t file_size;
} syn_t;

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
file_info_list_t* convert_to_file_list(data_buffer_t* data);
convert_helper_t convert_to_data(packet_t& packet);
convert_helper_t convert_to_data(packet_t const& packet);
convert_helper_t convert_to_data(handshake_t& hand);
convert_helper_t convert_to_data(ack_t& ack);
convert_helper_t convert_to_data(syn_t& syn);
convert_helper_t convert_to_data(file_info_list_t& list);
convert_helper_t convert_to_data(file_info_list_t const& list);
convert_helper_t convert_to_data(std::string string);

#endif // BROPBOXUTIL_HPP 