#pragma once

#include "networking/Socket.hpp"
#include "util/Definitions.hpp"

#include <optional>
#include <string>

#include <string.h>
#include <sys/sendfile.h>
#include <sys/uio.h>

#define TIMEOUT_SEC 0
#define TIMEOUT_USEC 200000
#define BUFFER_SIZE 32768

namespace networking {

class ActiveSocket : public Socket {
    sockaddr_in peer_address;

    /**
     * 
     */
    static std::optional<sockaddr_in> init_peer_addr(port_t port, const char* addr);

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
     * 
     */
    template <typename T1, typename T2>
    bool pair_send(const T1* first, const T2* second) const
    {
        iovec iov[2] = { { first, sizeof(T1) }, { second, sizeof(T2) } };

        auto written_bytes = writev(this->sock_fd, iov, 2);

        if (written_bytes == -1) {
            perror("writev");
            return false;
        } else if (written_bytes < sizeof(T1) + sizeof(T2)) {
            //TODO: log
            // unexpected short write
            return false;
        }

        return true;
    }

    template <typename T1, typename T2>
    std::pair<std::unique_ptr<T1>, std::unique_ptr<T2>> pair_recv() const
    {
        auto first = std::make_unique<T1>();
        auto second = std::make_unique<T2>();

        iovec iov[2] = { { first.get(), sizeof(T1) }, { second.get(), sizeof(T2) } };

        auto bytes_read = readv(this->sock_fd, iov, 2);

        if (bytes_read == -1) {
            perror("readv");
            return std::make_pair(nullptr, nullptr);
        } else if (bytes_read < sizeof(T1) + sizeof(T2)) {
            //TODO: log
            // unexpected short write
            return std::make_pair(nullptr, nullptr);
        }

        return std::make_pair(std::move(first), std::move(second));
    }

    /**
     * Sends BUFFER_SIZE bytes from an open file descriptor to the socket.
     * 
     * @param file_desc the file descriptor.
     * 
     * @return the number of bytes written to the socket or -1 in case of failure.
     */
    ssize_t send_file_block(int file_desc) const;

    /**
     * Receives BUFFER_SIZE bytes and writes them to a open file descriptor. This method
     * is designed to work as a pair of 'send_file', though you could use it to 
     * write character data received as a packet directly to a file.
     * 
     * @param file_desc the file descriptor.
     * @param size the informed length in bytes.
     * 
     * @return the number of bytes written to the file or -1 in case of failure.
     */
    ssize_t recv_file_block(int file_desc) const;

    /**
     * Changes the connected address of the socket.
     * 
     * @param the new peer address.
     */
    bool change_peer(const sockaddr_in& new_peer) noexcept;

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
