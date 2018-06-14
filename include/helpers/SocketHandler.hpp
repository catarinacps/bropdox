#pragma once

#include "util/Definitions.hpp"
#include "util/Exception.hpp"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>

#include <memory>
#include <iostream>
#include <queue>

#define TIMEOUT 500000

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

    /**
     * Sends a data packet to the last known client (aka the last client that the socket
     * heard from).
     * 
     * @param data a reference to the data pointer.
     * @param size the size of the said packet.
     * @param address the target address.
     * 
     * @return a boolean representing success (true) or failure (false).
     */
    bool send_packet(void* data, size_t size, sockaddr_in const& address) const;

    sockaddr_in get_last_address() const noexcept;

private:
    void log(char const* message) const;

    static int init_unix_socket(struct sockaddr_in& sock, port_t port);

    static int init_unix_socket(struct sockaddr_in& sock, port_t port, hostent& server);

public:
    /**
     * The one used by the client
     */
    SocketHandler(port_t port, hostent* server);

    /**
     * The one used by the RequestHandler
     */
    SocketHandler(port_t port, sockaddr_in peer_address_buffer);

    /**
     * The one used by the server
     */
    SocketHandler(port_t port);

    SocketHandler() {}

    ~SocketHandler();
};
