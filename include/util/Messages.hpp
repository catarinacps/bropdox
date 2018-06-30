#pragma once

#include "util/Definitions.hpp"

#include <cstring>
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
}
