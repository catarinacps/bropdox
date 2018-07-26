#pragma once

#include "networking/Socket.hpp"
#include "util/Definitions.hpp"

namespace networking {

class RawSocket : public Socket {
public:
    /**
     * Tries to read the socket for a sizeof(T) bytes datagram.
     * This method is a blocking call if there's no data in the socket. If theres more bytes in the 
     * actual datagram than the size of T, the remaining bytes will get truncated. If the datagram 
     * is smaller than T, all read data gets dropped.
     * As a warning, theres maybe a chance that the whole package didn't got read. In this case, all
     * unread bytes will get silently discarted.
     * 
     * @return a pair containing a unique_ptr<T> and the address of the author of the message.
     */
    template <typename T>
    std::pair<std::unique_ptr<T>, sockaddr_in> recv_data() const
    {
        auto datagram = std::make_unique<T>();
        sockaddr_in author{};

        auto read_bytes = recvfrom(this->sock_fd, datagram.get(), sizeof(T), 0, (sockaddr*)&author, sizeof(sockaddr_in));

        if (read_bytes == -1) {
            perror("recvfrom");
            return std::make_pair(nullptr, sockaddr_in());
        } else if (read_bytes < sizeof(T)) {
            //TODO: log
            // invalid or mismatched packet size read
            return std::make_pair(nullptr, sockaddr_in());
        }

        return std::make_pair(std::move(datagram), author);
    }

    /**
     * Tries to send the pointed data as a T-sized datagram to the socket.
     * This method will block if the socket's buffer has overflown, otherwise it is a non-blocking
     * call.
     * 
     * @param data a const pointer of type T to the data.
     * @param address the recipient's address
     * 
     * @return a boolean representing success or failure.
     */
    template <typename T>
    bool send_data(const T* data, const sockaddr_in& address) const
    {
        auto sent_bytes = sendto(this->sock_fd, data, sizeof(T), 0, (sockaddr*)&address, sizeof(sockaddr_in));

        if (sent_bytes == -1) {
            perror("sendto");
            return false;
        } else if (sent_bytes < sizeof(T)) {
            //TODO: log
            // less bytes sent than expected
            return false;
        }

        return true;
    }

    /**
     * Creates an active internet datagram socket.
     */
    RawSocket();

    /**
     * Moves (and creates) an active internet datagram socket.
     * 
     * @param the expiring RawSocket.
     */
    RawSocket(RawSocket&&);

    /**
     * No two exactly equal sockets can exist at a given instant.
     */
    RawSocket(const RawSocket&) = delete;

    /**
     * Closes the socket if the socket file descriptor is valid.
     */
    ~RawSocket();
};
}