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
    bool send_packet(void* data, size_t size);

    sockaddr_in get_last_peeraddr();

private:
    int sockfd;
    socklen_t peer_len;
    struct sockaddr_in handler_address, peer_address;

public:
    /**
     * The one used by the client
     */
    SocketHandler(in_port_t port, hostent* server);

    /**
     * The one used by the RequestHandler
     */
    SocketHandler(in_port_t port, sockaddr_in peer_address);

    /**
     * The one used by the server
     */
    SocketHandler(in_port_t port);

    ~SocketHandler();
};

#endif // SOCKETHANDLER_HPP
