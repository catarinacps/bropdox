#include "../include/PortManager.hpp"

/***************************************************************************************************
 * PUBLIC
 */

PortManager(port_t port)
    : server_port(port)
    , port_map(MAXPORT - port, false)
{
}

port_t PortManager::reserve_port()
{
    throw bdu::not_implemented();
}

void PortManager::free_port(port_t port)
{
    throw bdu::not_implemented();
}
