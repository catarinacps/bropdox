#pragma once

#include "util/Exception.hpp"
#include "util/Definitions.hpp"

#include <mutex>
#include <vector>

#define MAXPORT 65535

class PortManager {
    port_t const server_port;
    std::vector<bool> port_map;

    std::mutex mutable m_port;

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
    bool free_port(port_t port);

    PortManager(port_t port);
};
