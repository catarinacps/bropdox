#pragma once

#include "helpers/SocketHandler.hpp"
#include "util/Definitions.hpp"

#include <netinet/in.h>

#include <cstring>
#include <map>
#include <string>

#ifndef MAXNAME
#define MAXNAME 255
#endif

#define PACKETSIZE 16384

/******************************************************************************
 * Message structs
 */

namespace bdu {

enum class req {
    sync,
    send,
    receive,
    del,
    login,
    list,
    close
};

enum class serv_req {
    sync,
    alive,
    client_login,
    propagate_login,
    election,
    new_member,
    request_entrance
};

struct handshake_t {
    req req_type;
    char userid[MAXNAME];
    device_t device;

    handshake_t(req request, char const* id, device_t dev = 0)
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
    device_t device;

    syn_t(bool conf_p, port_t port_p, device_t device_p)
        : confirmation(conf_p)
        , port(port_p)
        , device(device_p)
    {
    }

    syn_t() = default;
};

struct packet_t {
    unsigned int num;
    unsigned int data_size;
    byte_t data[PACKETSIZE];

    packet_t(unsigned int num_p, unsigned int data_size_p)
        : num(num_p)
        , data_size(data_size_p)
    {
    }

    packet_t() = default;
};

struct rm_syn_t {
    id_t id;

    rm_syn_t(id_t i)
        : id(i)
    {
    }

    rm_syn_t() = default;
};

struct rm_operation_t {
    serv_req req;

    rm_operation_t(serv_req req_p)
        : req(req_p)
    {
    }

    rm_operation_t() = default;
};

struct member_t {
    id_t id;
    sockaddr_in address;

    member_t(id_t id_p, sockaddr_in addr)
        : id(id_p)
        , address(addr)
    {
    }

    member_t() = default;
};

struct client_t {
    char name[MAXNAME];

    client_t(std::string name_p)
        : client_t()
    {
        std::strcpy(name, name_p.c_str());
    }

    client_t()
        : name{ '\0' }
    {
    }
};
}