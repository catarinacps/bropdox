#include "networking/PassiveSocket.hpp"

namespace networking {

PassiveSocket::PassiveSocket(port_t peer_port)
{
    this->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (this->sock_fd == -1) {
        perror("socket");
        throw bdu::socket_bad_create();
    }

    if (!init_own_addr(this->own_address, peer_port)) {
        //TODO: log
        // error on addr init
        throw bdu::socket_bad_create();
    }

    if (!bind_to(this->own_address)) {
        //TODO: log
        // error on connect
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

void PassiveSocket::init_own_addr(sockaddr_in& own_addr, port_t port)
{
    memset(&own_addr, 0, sizeof(sockaddr_in));
    own_addr.sin_family = AF_INET;
    own_addr.sin_addr.s_addr = INADDR_ANY;
    own_addr.sin_port = htons(port);
}
}