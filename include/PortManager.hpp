#pragma once

#include "Exception.hpp"
#include "bropdoxUtil.hpp"

#include <mutex>
#include <netdb.h>
#include <vector>

#define MAXPORT 65535

class PortManager {
    port_t const server_port;
    std::vector<bool> port_map;

    std::mutex m_port;

public:
    /**
     * Tries to reserve the use of a given port.
     * 
     * @return the reserved port
     */
    port_t reserve_port();

    /**
     * Tries to free the given port number.
     * 
     * @param port the said port
     */
    void free_port(port_t port);

    PortManager(port_t port);
};
