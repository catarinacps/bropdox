#pragma once

#include "util/Definitions.hpp"

#include <cstring>
#include <string>

#ifndef PACKETSIZE
#define PACKETSIZE 16384
#endif

#ifndef MAXNAME
#define MAXNAME 255
#endif

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
};

struct ack_t {
    bool confirmation;

    ack_t(bool conf)
        : confirmation(conf)
    {
    }
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
};

struct packet_t {
    unsigned int num;
    byte_t data[PACKETSIZE];

    packet_t(unsigned int num_p)
        : num(num_p)
    {
    }
};
}
