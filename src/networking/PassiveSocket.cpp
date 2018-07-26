#include "networking/PassiveSocket.hpp"

namespace networking {

PassiveSocket::PassiveSocket(port_t peer_port)
{
    this->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (this->sock_fd == -1) {
        perror("socket");
        throw bdu::socket_bad_create();
    }

    this->own_address = this->init_own_addr(peer_port);

    if (!this->bind_to(this->own_address)) {
        //TODO: log
        // error on bind_to
        throw bdu::socket_bad_create();
    }
}

PassiveSocket::PassiveSocket(PassiveSocket&& move)
{
    this->sock_fd = move.sock_fd;
    this->own_address = move.own_address;

    move.sock_fd = -1;
}

PassiveSocket::~PassiveSocket()
{
    if (this->sock_fd > 0) {
        close(this->sock_fd);
    }
}

sockaddr_in PassiveSocket::init_own_addr(port_t port)
{
    sockaddr_in my_address{};

    // memset(&my_address, 0, sizeof(sockaddr_in));
    my_address.sin_family = AF_INET;
    my_address.sin_addr.s_addr = INADDR_ANY;
    my_address.sin_port = htons(port);

    return my_address;
}
}