#pragma once

#include "util/Definitions.hpp"
#include "util/Exception.hpp"
#include "util/Convert.hpp"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>

#include <iostream>
#include <memory>
#include <queue>
#include <map>

#define TIMEOUT 500000

class SocketHandler {
    int sockfd;
    socklen_t peer_len;
    struct sockaddr_in handler_address;
    struct sockaddr_in peer_address;

public:
    /**
     * Listens the socket for a incoming packet, expecting it to be of type T.
     * 
     * @return a unique_ptr of T.
     */
    template <typename T>
    std::unique_ptr<T> wait_packet()
    {
        auto packet = std::make_unique<T>();

        int desc = recvfrom(this->sockfd, (void*)packet.get(), sizeof(T), 0, (struct sockaddr*)&(this->peer_address), &(this->peer_len));
        if (desc < 0) {
            this->log("Error while receiving packet...");
            perror("wait_packet error");
            return nullptr;
        }

        //! Caller will now own the buffer
        this->log("Received a packet");
        return packet;
    }

    /**
     * Sends a data packet to the last known client (aka the last client that the socket
     * heard from).
     * 
     * @param data a reference to the data pointer.
     * 
     * @return a boolean representing success (true) or failure (false).
     */
    template <typename T>
    bool send_packet(T const* data) const
    {
        auto addr_size = sizeof(struct sockaddr_in);
        int desc = sendto(this->sockfd, data, sizeof(T), 0, (struct sockaddr*)&(this->peer_address), addr_size);
        if (desc < 0) {
            this->log("Error while sending packet...");
            perror("send_packet error");
            return false;
        }

        this->log("Sent a packet");
        return true;
    }

    /**
     * Sends a data packet to the specified target address.
     * 
     * @param data a reference to the data pointer.
     * @param address the target address.
     * 
     * @return a boolean representing success (true) or failure (false).
     */
    template <typename T>
    bool send_packet(T const* data, sockaddr_in const& address) const
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
     * Multicasts a data packet to all targets in the specified map parameter.
     * 
     * @param data A reference to the data pointer.
     * @param targets A map containing all target sockaddr.
     * 
     * @return a boolean representing success if every packet was delivered successfully or 
     * false if some packet failed to be delivered.
     */
    template <typename T>
    bool multicast_packet(T const* data, std::map<id_t, sockaddr_in> const& targets) const
    {
        bool success = true;
        
        for (auto const& pair : targets) {
            if (!this->send_packet(data, pair.second)) {
                success = false;
            }
        }

        return success;
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

    SocketHandler(SocketHandler const& copy) = delete;

    SocketHandler(SocketHandler&& move);

    ~SocketHandler();

    SocketHandler& operator=(SocketHandler&& move);
};
