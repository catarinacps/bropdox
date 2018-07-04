#pragma once

#include "helpers/SocketHandler.hpp"
#include "util/Messages.hpp"

#include <thread>

class FrontEnd{

    SocketHandler sock_handler;
    
    sockaddr_in primary_address;
public:
    FrontEnd(port_t port_p, char const* host);

    void run();

    template <typename T>
    bool send_packet(T const* data) const{
        return this->sock_handler.send_packet(data, this->primary_address);
    }

     template <typename T>
    std::unique_ptr<T> wait_packet()
    {
        return this->sock_handler.wait_packet<T>();
    }
    
private:
    void change_primary(sockaddr_in address);
    
    void listen();


};