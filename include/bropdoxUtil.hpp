#ifndef BROPBOXUTIL_HPP
#define BROPBOXUTIL_HPP

#ifndef BOOST_ALL_DYN_LINK
#define BOOST_ALL_DYN_LINK
#endif

#define MAXNAME 255
#define MAXFILES 65536
#define PACKETSIZE 16384

#define MAX_FILE_LIST_SIZE 10

#define DAEMON_SLEEP 10000000
#define TIMEOUT 500000

#define ADDR "BropDoxServer"

#define PORT 4000
#define MAXPORT 65535

#include <boost/filesystem.hpp>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <string>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

namespace bf = boost::filesystem;

/******************************************************************************
 * Types
 */

enum class req { 
    sync,
    send,
    receive,
    del,
    close
};

typedef unsigned char data_buffer_t;

struct file_info {
    char name[MAXNAME * 2];
    char last_modified[MAXNAME];
    int size;

    file_info(std::string name_p, std::string sync_dir)
    : name{ '\0' }, last_modified{ '\0' }, size(0)
    {
        time_t last_time;

        if (bf::exists(sync_dir + name_p)) {
            last_time = bf::last_write_time(sync_dir + name_p);
            size = bf::file_size(sync_dir + name_p);
            std::strcpy(last_modified, asctime(gmtime(&last_time)));
        }

        std::strcpy(name, name_p.c_str());
    }

    file_info()
    : name{ '\0' }, last_modified{ '\0' }, size(0)
    {
    }

    bool operator<(file_info const& a) const
    {
        return name < a.name;
    }
};

typedef struct {
    file_info file_list[MAX_FILE_LIST_SIZE];
    bool has_next;
} file_info_list_t;

typedef struct handshake {
    req req_type;
    char userid[MAXNAME];

    handshake(req request, char const* id)
    : req_type(request), userid{ '\0' }
    {
        std::string aux(id);

        // Trivial
        // Proof is left as an exercise to the reader
        std::strcpy(userid, aux.substr(0, aux.find_first_of('\0')).c_str());
    }

    handshake()
    : userid{ '\0' }
    {
    }
} handshake_t;

typedef struct ack {
    bool confirmation;

    ack(bool conf)
    : confirmation(conf)
    {
    }

    ack()
    {
    }
} ack_t;

typedef struct syn {
    bool confirmation;
    in_port_t port;

    syn(bool conf, in_port_t port_p)
    : confirmation(conf), port(port_p)
    {
    }

    syn()
    {
    }
} syn_t;

typedef struct file_data {
    struct file_info file;
    unsigned int num_packets;

    file_data(file_info file_p, unsigned int packets)
    : file(file_p), num_packets(packets)
    {
    }

    file_data()
    {
    }
} file_data_t;

typedef struct packet {
    unsigned int num;
    data_buffer_t data[PACKETSIZE];

    packet(unsigned int num_p)
    : num(num_p)
    {
    }

    packet()
    {
    }
} packet_t;

typedef struct {
    data_buffer_t* pointer;
    size_t size;
} convert_helper_t;

/******************************************************************************
 * Headers
 */

int init_unix_socket(struct sockaddr_in& sock, in_port_t port);
int init_unix_socket(struct sockaddr_in& sock, in_port_t port, hostent* server);

handshake_t* convert_to_handshake(data_buffer_t* data);
ack_t* convert_to_ack(data_buffer_t* data);
syn_t* convert_to_syn(data_buffer_t* data);
packet_t* convert_to_packet(data_buffer_t* data);
file_info_list_t* convert_to_file_list(data_buffer_t* data);
file_data_t* convert_to_file_data(data_buffer_t* data);
convert_helper_t convert_to_data(packet_t& packet);
convert_helper_t convert_to_data(packet_t const& packet);
convert_helper_t convert_to_data(handshake_t& hand);
convert_helper_t convert_to_data(ack_t& ack);
convert_helper_t convert_to_data(syn_t& syn);
convert_helper_t convert_to_data(file_info_list_t& list);
convert_helper_t convert_to_data(file_info_list_t const& list);
convert_helper_t convert_to_data(std::string string);

#endif // BROPBOXUTIL_HPP