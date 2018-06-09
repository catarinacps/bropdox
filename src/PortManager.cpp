#include "../include/PortManager.hpp"

/***************************************************************************************************
 * PUBLIC
 */

PortManager(port_t const port)
    : server_port(port)
    , port_map(MAXPORT - port, false)
{
}

port_t PortManager::reserve_port()
{
    auto i = 1;

    this->m_port.lock();

    for (auto&& occupied : this->port_counter) {
        if (!occupied) {
            occupied = true;

            return this->server_port + i;
        }
        i++;
    }

    this->m_port.unlock();

    return 0;
}

bool PortManager::free_port(port_t port)
{
    auto& port = this->port_map.at(this->server_port - port - 1);

    if (port == false) {
        return false;
    }

    port = false;
    return true;
}