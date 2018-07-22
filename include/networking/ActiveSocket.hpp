#pragma once

#include "networking/Socket.hpp"
#include "util/Definitions.hpp"

#include <string>

#include <arpa/inet.h>
#include <string.h>
#include <sys/sendfile.h>

#define TIMEOUT_SEC 0
#define TIMEOUT_USEC 200000
#define BUFFER_SIZE 32768

namespace networking {

class ActiveSocket : public Socket {
    sockaddr_in peer_address;

    static bool init_peer_addr(sockaddr_in& peer, port_t port, const char* addr);

public:
    /**
     * Tries to read the socket for a sizeof(T) bytes datagram.
     * This method is a blocking call if there's no data in the socket. If theres more bytes in the 
     * actual datagram than the size of T, the remaining bytes will get truncated. If the datagram 
     * is smaller than T, all read data gets dropped.
     * As a warning, theres maybe a chance that the whole package didn't got read. In this case, all
     * unread bytes will get silently discarted.
     * 
     * @return a unique_ptr containing the read T.
     */
    template <typename T>
    std::unique_ptr<T> recv_data() const
    {
        auto datagram = std::make_unique<T>();

        auto read_bytes = recv(this->sock_fd, datagram.get(), sizeof(T), 0);

        if (read_bytes == -1) {
            perror("recv");
            return nullptr;
        } else if (read_bytes < sizeof(T)) {
            //TODO: log
            // invalid or mismatched packet size read
            return nullptr;
        }

        return datagram;
    }

    /**
     * Tries to send the pointed data as a T-sized datagram to the socket.
     * This method will block if the socket's buffer has overflown, otherwise it is a non-blocking
     * call.
     * 
     * @param data a const pointer of type T to the data.
     * 
     * @return a boolean representing success or failure.
     */
    template <typename T>
    bool send_data(const T* data) const
    {
        auto sent_bytes = send(this->sock_fd, data, sizeof(T), 0);

        if (sent_bytes == -1) {
            perror("send");
            return false;
        } else if (sent_bytes < sizeof(T)) {
            //TODO: log
            // less bytes sent than expected
            return false;
        }

        return true;
    }

    /**
     * Sends 'n' bytes from an open file descriptor.
     * 
     * @param file_desc the file descriptor.
     * 
     * @return the number of bytes written to the socket or -1 in case of failure.
     */
    ssize_t send_file(int file_desc);

    /**
     * Receives 'n' bytes and writes them to a open file descriptor. This method
     * is designed to work as a pair of 'send_file', though you could use it to 
     * write character data received as a packet directly to a file.
     * 
     * @param file_desc the file descriptor.
     * @param size the informed length in bytes.
     * 
     * @return the number of bytes written to the file or -1 in case of failure.
     */
    ssize_t recv_file(int file_desc);

    /**
     * Creates an active internet datagram socket.
     * 
     * @param peer_port the peer's port.
     * @param peer_addr the peer's IPv4 address in humman readable form.
     */
    ActiveSocket(port_t peer_port, const std::string& peer_addr);

    /**
     * Creates an active internet datagram socket.
     * 
     * @param peer a sockaddr_in with the peer's address.
     */
    ActiveSocket(const sockaddr_in& peer);

    /**
     * Moves (and creates) an active internet datagram socket.
     * 
     * @param the expiring ActiveSocket.
     */
    ActiveSocket(ActiveSocket&&);

    /**
     * No two exactly equal sockets can exist at a given instant.
     */
    ActiveSocket(const ActiveSocket&) = delete;

    /**
     * No uninitialized socket can exist at a given time.
     */
    ActiveSocket() = delete;

    /**
     * Closes the socket if the socket file descriptor is valid.
     */
    ~ActiveSocket();
};
}