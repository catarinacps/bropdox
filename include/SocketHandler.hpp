#ifndef SOCKETHANDLER_HPP
#define SOCKETHANDLER_HPP

#include "bropdoxUtil.hpp"

#include <iostream>
#include <queue>

class SocketHandler {
    int sockfd;
    socklen_t peer_len;
    struct sockaddr_in handler_address;
    struct sockaddr_in peer_address_buffer;

    std::queue<sockaddr_in> peer_address_queue;

public:
    /**
     * Listens the socket for incoming packets, expecting them with a predetermined size.
     * 
     * @param size the size of the expected packet.
     * 
     * @return a unique_ptr to the byte array.
     */
    std::unique_ptr<byte_t[]> wait_packet(size_t size);

    /**
     * Sends a data packet to the last known client (aka the last client that the socket
     * heard from).
     * 
     * @param data a reference to the data pointer.
     * @param size the size of the said packet.
     * 
     * @return a boolean representing success (true) or failure (false).
     */
    bool send_packet(void* data, size_t size) const;

    sockaddr_in pop_peer_address() const;

    void flush_address_queue() noexcept;

private:
    
    void log(char const* message) const;

public:
    /**
     * The one used by the client
     */
    SocketHandler(in_port_t port, hostent* server);

    /**
     * The one used by the RequestHandler
     */
    SocketHandler(in_port_t port, sockaddr_in peer_address_buffer);

    /**
     * The one used by the server
     */
    SocketHandler(in_port_t port);

    SocketHandler() {}

    ~SocketHandler();
};

#endif // SOCKETHANDLER_HPP
