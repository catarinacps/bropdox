#include "../include/SocketHandler.hpp"

/******************************************************************************
 * Constructors
 */

SocketHandler::SocketHandler(in_port_t port, hostent* server) throw()
{
    struct timeval timeout = { 0, TIMEOUT };

    this->peer_len = sizeof(struct sockaddr_in);

    try {
        this->sockfd = init_unix_socket(this->handler_address, port);
    } catch (socket_bad_create const& e) {
        throw e;
    }

    if (setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval)) < 0) {
        this->log("Error while setting the timeout, please try again...");
        throw socket_bad_opt();
    }
}

SocketHandler::SocketHandler(in_port_t port, sockaddr_in peer_address) throw()
{
    struct timeval timeout = { 0, TIMEOUT };

    this->peer_address = peer_address;
    this->peer_len = sizeof(struct sockaddr_in);

    try {
        this->sockfd = init_unix_socket(this->handler_address, port);
    } catch (socket_bad_create const& e) {
        throw e;
    }

    if (setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval)) < 0) {
        this->log("Error while setting the timeout, please try again...");
        throw socket_bad_opt();
    }

    if (bind(this->sockfd, (struct sockaddr*)&(this->handler_address), sizeof(struct sockaddr)) < 0) {
        this->log("Error while binding the socket, please try again...");
        throw socket_bad_bind();
    }
}

SocketHandler::SocketHandler(in_port_t port) throw()
{
    this->peer_len = sizeof(struct sockaddr_in);

    try {
        this->sockfd = init_unix_socket(this->handler_address, port);
    } catch (socket_bad_create const& e) {
        throw e;
    }

    if (bind(sockfd, (struct sockaddr*)&(this->handler_address), sizeof(struct sockaddr)) < 0) {
        this->log("Error while binding the socket, please try again...");
        throw socket_bad_bind();
    }
}

/******************************************************************************
 * Member Functions
 */

std::unique_ptr<byte_t[]> SocketHandler::wait_packet(size_t size)
{
    auto buffer = std::make_unique<byte_t[]>(size);

    int desc = recvfrom(this->sockfd, (void*)buffer.get(), size, 0, (struct sockaddr*)&(this->peer_address), &(this->peer_len));
    if (desc < 0) {
        this->log("Error while receiving packet...");
        return nullptr;
    }

    //! Caller will now own the buffer
    return buffer;
}

bool SocketHandler::send_packet(void* data, size_t size) const
{
    int desc = sendto(this->sockfd, data, size, 0, (struct sockaddr*)&(this->peer_address), sizeof(struct sockaddr_in));
    if (desc < 0) {
        this->log("Error while sending packet...");
        return false;
    }

    return true;
}

sockaddr_in SocketHandler::get_last_peeraddr() const
{
    return this->peer_address;
}

void SocketHandler::log(char const* message) const
{
    printf("SocketHandler [sockfd: %d]: %s\n", this->sockfd, message);
}

SocketHandler::~SocketHandler()
{
    close(this->sockfd);
}