#include "client/FrontEnd.hpp"

FrontEnd::FrontEnd(port_t port_p, hostent* host)
    : sock_handler_server(port_p, host)
    , sock_handler_fe(port_p - 1, host)
    , primary_address(this->sock_handler_server.get_last_address())
{
}

void FrontEnd::change_primary(sockaddr_in address)
{
    this->primary_address = address;
}

void FrontEnd::listen()
{
    while (true) {
        auto addr = this->sock_handler_fe.wait_packet<bdu::address_t>();

        if (addr) {
            this->change_primary(addr->address);
        }
    }
}

void FrontEnd::run()
{
    std::thread primary_listener(&FrontEnd::listen, this);

    if (!primary_listener.joinable()) {
        printf("Failed to create the 'listen' thread");

        return;
    }

    primary_listener.detach();
}
