#ifndef SOCKETHANDLER_HPP
#define SOCKETHANDLER_HPP

#include "bropdoxUtil.hpp"

class SocketHandler {
public:
    /**
     * Listens the socket for incoming packets, expecting them with a predetermined size.
     * 
     * @param size the size of the expected packet.
     * 
     * @return a pointer to the data buffer.
     */
    data_buffer_t* wait_packet(size_t size);

    /**
     * Sends a data packet to the last known client (aka the last client that the socket
     * heard from).
     * 
     * @param data a reference to the data buffer.
     * @param size the size of the said packet.
     * 
     * @return a boolean representing success (true) or failure (false).
     */
    bool send_packet(data_buffer_t* data, size_t size);

    sockaddr_un get_last_clientaddr();

private:
    int sockfd;
    socklen_t client_len;
    struct sockaddr_un handler_address, client_address;

public:
    SocketHandler(sockaddr_un client_addr, std::string address = "");
    ~SocketHandler();
};

#endif // SOCKETHANDLER_HPP