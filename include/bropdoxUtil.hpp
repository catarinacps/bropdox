#pragma once

#ifndef BOOST_ALL_DYN_LINK
#define BOOST_ALL_DYN_LINK
#endif

#define MAXNAME 255
#define MAXFILES 65536
#define PACKETSIZE 16384

#define MAX_FILE_LIST_SIZE 10

#define MAX_CONCURRENT_USERS 2

#define DAEMON_SLEEP 10000000
#define TIMEOUT 500000

#define MAXPORT 65535

#include "Exception.hpp"

#include <boost/filesystem.hpp>

#include <chrono>
#include <memory>
#include <string>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
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
    login,
    close
};

using byte_t = unsigned char;

using device_t = unsigned short int;

using port_t = unsigned int;

struct file_info {
    char name[MAXNAME * 2];
    char last_modified[MAXNAME];
    int size;

    file_info(std::string const& name_p, std::string const& sync_dir)
        : name{ '\0' }
        , last_modified{ '\0' }
        , size(0)
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
        : name{ '\0' }
        , last_modified{ '\0' }
        , size(0)
    {
    }

    bool operator<(file_info const& a) const
    {
        return name < a.name;
    }
};

struct handshake_t {
    req req_type;
    char userid[MAXNAME];
    unsigned short int device;

    handshake_t(req request, char const* id, unsigned short int dev = 0)
        : req_type(request)
        , userid{ '\0' }
        , device(dev)
    {
        std::string aux(id);

        // Trivial
        // Proof is left as an exercise to the reader
        std::strcpy(userid, aux.substr(0, aux.find_first_of('\0')).c_str());
    }

    handshake_t() = default;
};

struct ack_t {
    bool confirmation;

    ack_t(bool conf)
        : confirmation(conf)
    {
    }

    ack_t() = default;
};

struct syn_t {
    bool confirmation;
    port_t port;
    unsigned short int device;

    syn_t(bool conf_p, port_t port_p, unsigned short int device_p)
        : confirmation(conf_p)
        , port(port_p)
        , device(device_p)
    {
    }

    syn_t() = default;
};

struct file_data_t {
    struct file_info file;
    unsigned int num_packets;

    file_data_t(file_info const& file_p, unsigned int packets)
        : file(file_p)
        , num_packets(packets)
    {
    }

    file_data_t() = default;
};

struct file_info_list_t {
    file_data_t file_list[MAX_FILE_LIST_SIZE];
    bool has_next;
};

struct packet_t {
    unsigned int num;
    byte_t data[PACKETSIZE];

    packet_t(unsigned int num_p)
        : num(num_p)
    {
    }

    packet_t() = default;
};

struct convert_helper_t {
    byte_t* pointer;
    size_t size;
};

/******************************************************************************
 * Headers
 */

int init_unix_socket(struct sockaddr_in& sock, port_t port);
int init_unix_socket(struct sockaddr_in& sock, port_t port, hostent* server);

std::unique_ptr<handshake_t> convert_to_handshake(byte_t* data);
std::unique_ptr<ack_t> convert_to_ack(byte_t* data);
std::unique_ptr<syn_t> convert_to_syn(byte_t* data);
std::unique_ptr<packet_t> convert_to_packet(byte_t* data);
std::unique_ptr<file_info_list_t> convert_to_file_list(byte_t* data);
std::unique_ptr<file_data_t> convert_to_file_data(byte_t* data);
