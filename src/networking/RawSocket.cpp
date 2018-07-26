#include "networking/RawSocket.hpp"

namespace networking {

RawSocket::RawSocket()
{
    this->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (this->sock_fd == -1) {
        perror("socket");
        throw bdu::socket_bad_create();
    }

    auto address = this->get_own_address();
    if (!address) {
        throw bdu::socket_bad_create();
    }

    this->own_address = address.value();
}

RawSocket::RawSocket(RawSocket&& move)
{
    this->sock_fd = move.sock_fd;
    this->own_address = move.own_address;

    move.sock_fd = -1;
}

RawSocket::~RawSocket()
{
    if (this->sock_fd > 0) {
        close(this->sock_fd);
    }
}
}