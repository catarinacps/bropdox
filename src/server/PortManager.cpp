#include "server/PortManager.hpp"

/***************************************************************************************************
 * PUBLIC
 */

PortManager::PortManager(port_t port)
    : server_port(port)
    , port_map(MAXPORT - port, false)
{
}

port_t PortManager::reserve_port()
{
    auto i = 1;

    this->m_port.lock();

    for (auto&& occupied : this->port_map) {
        if (!occupied) {
            occupied = true;

            this->m_port.unlock();
            return this->server_port + i;
        }
        i++;
    }

    this->m_port.unlock();

    return 0;
}

bool PortManager::free_port(port_t port)
{
    auto ref = this->port_map.at(this->server_port - port - 1);

    if (ref == false) {
        return false;
    }

    ref = false;
    return true;
}