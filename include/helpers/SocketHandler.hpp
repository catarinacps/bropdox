#pragma once

#include "util/Definitions.hpp"
#include "util/Exception.hpp"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>

#include <iostream>
#include <memory>
#include <queue>

#define TIMEOUT 500000

class SocketHandler {
    int sockfd;
    socklen_t peer_len;
    struct sockaddr_in handler_address;
    struct sockaddr_in peer_address;

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
     * 
     * @return a boolean representing success (true) or failure (false).
     */
    template <typename T>
    bool send_packet(T* data) const
    {
        int desc = sendto(this->sockfd, data, sizeof(T), 0, (struct sockaddr*)&(this->peer_address), sizeof(struct sockaddr_in));
        if (desc < 0) {
            this->log("Error while sending packet...");
            perror("send_packet error");
            return false;
        }

        this->log("Sent a packet");
        return true;
    }

    /**
     * Sends a data packet to the last known client (aka the last client that the socket
     * heard from).
     * 
     * @param data a reference to the data pointer.
     * @param address the target address.
     * 
     * @return a boolean representing success (true) or failure (false).
     */
    template <typename T>
    bool send_packet(T* data, sockaddr_in const& address) const
    {
        int desc = sendto(this->sockfd, data, sizeof(T), 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in));
        if (desc < 0) {
            this->log("Error while sending packet...");
            perror("send_packet error");
            return false;
        }

        this->log("Sent a packet");
        return true;
    }

    /**
     * Gets the last address in the peer address buffer.
     * 
     * @return the peer address
     */
    sockaddr_in get_last_address() const noexcept;

    /**
     * Logs a message to stdout using the context of the object.
     */
    void log(char const* message) const;

private:
    static int init_server_socket(struct sockaddr_in& sock, port_t port);

    static int init_client_socket(struct sockaddr_in& sock, port_t port, hostent* server);

public:
    /**
     * The one used by the client
     */
    SocketHandler(port_t port, hostent* server);

    /**
     * The one used by the RequestHandler
     */
    SocketHandler(port_t port, sockaddr_in peer_address_p);

    /**
     * The one used by the server
     */
    SocketHandler(port_t port);

    SocketHandler() {}

    SocketHandler(SocketHandler const& copy) = default;

    SocketHandler(SocketHandler&& move);

    ~SocketHandler();

    SocketHandler& operator=(SocketHandler&& move);
};
