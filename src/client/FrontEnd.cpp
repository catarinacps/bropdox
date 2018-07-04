#include "client/FrontEnd.hpp"

FrontEnd::FrontEnd(port_t port_p, char const* host)
    : sock_handler(port_p, host)
    , primary_address(this->sock_handler.get_last_address())
{    
}

void FrontEnd::change_primary(sockaddr_in address){
    this->primary_address = address;
}

void FrontEnd::listen(){
    //uhuul
    auto addr = this->sock_handler.wait_packet<bdu::fe_change_primary_t>();

    change_primary(addr->address);
}