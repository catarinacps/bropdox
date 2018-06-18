#include "helpers/SocketHandler.hpp"

/******************************************************************************
 * Constructors
 */

SocketHandler::SocketHandler(port_t port, hostent* server)
{
    struct timeval timeout = { 0, TIMEOUT };

    this->peer_len = sizeof(struct sockaddr_in);

    this->sockfd = init_unix_socket(this->handler_address, port, *server);

    if (setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval)) < 0) {
        this->log("Error while setting the timeout, please try again...");
        throw bdu::socket_bad_opt();
    }
}

SocketHandler::SocketHandler(port_t port, sockaddr_in peer_address_buffer)
{
    struct timeval timeout = { 0, TIMEOUT };

    this->peer_address_buffer = peer_address_buffer;
    this->peer_len = sizeof(struct sockaddr_in);

    this->sockfd = init_unix_socket(this->handler_address, port);

    if (setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval)) < 0) {
        this->log("Error while setting the timeout, please try again...");
        throw bdu::socket_bad_opt();
    }

    if (bind(this->sockfd, (struct sockaddr*)&(this->handler_address), sizeof(struct sockaddr)) < 0) {
        this->log("Error while binding the socket, please try again...");
        throw bdu::socket_bad_bind();
    }
}

SocketHandler::SocketHandler(port_t port)
{
    this->peer_len = sizeof(struct sockaddr_in);

    this->sockfd = init_unix_socket(this->handler_address, port);
    
    if (bind(sockfd, (struct sockaddr*)&(this->handler_address), sizeof(struct sockaddr)) < 0) {
        this->log("Error while binding the socket, please try again...");
        throw bdu::socket_bad_bind();
    }
}

/******************************************************************************
 * Member Functions
 */

std::unique_ptr<byte_t[]> SocketHandler::wait_packet(size_t size)
{
    auto buffer = std::make_unique<byte_t[]>(size);

    int desc = recvfrom(this->sockfd, (void*)buffer.get(), size, 0, (struct sockaddr*)&(this->peer_address_buffer), &(this->peer_len));
    if (desc < 0) {
        this->log("Error while receiving packet...");
        return nullptr;
    }

    this->peer_address_queue.push(this->peer_address_buffer);

    //! Caller will now own the buffer
    this->log("Received a packet");
    return buffer;
}

bool SocketHandler::send_packet(void* data, size_t size) const
{
    int desc = sendto(this->sockfd, data, size, 0, (struct sockaddr*)&(this->peer_address_buffer), sizeof(struct sockaddr_in));
    if (desc < 0) {
        this->log("Error while sending packet...");
        return false;
    }

    this->log("Sent a packet");
    return true;
}

bool SocketHandler::send_packet(void* data, size_t size, sockaddr_in const& address) const
{
    int desc = sendto(this->sockfd, data, size, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in));
    if (desc < 0) {
        this->log("Error while sending packet...");
        return false;
    }

    this->log("Sent a packet");
    return true;
}

sockaddr_in SocketHandler::get_last_address() const noexcept
{
    return this->peer_address_buffer;
}

void SocketHandler::log(char const* message) const
{
    printf("SocketHandler [sockfd: %d]: %s\n", this->sockfd, message);
}

int SocketHandler::init_unix_socket(struct sockaddr_in& sock, port_t port)
{
    int socket_id;
    if ((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Error while initializing the socket\n");
        throw bdu::socket_bad_create();
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = INADDR_ANY;
    std::memset(&(sock.sin_zero), 0, sizeof(sock.sin_zero));

    return socket_id;
}

int SocketHandler::init_unix_socket(struct sockaddr_in& sock, port_t port, hostent& server)
{
    int socket_id;
    if ((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Error while initializing the socket\n");
        throw bdu::socket_bad_create();
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr = *((struct in_addr*)server.h_addr);
    std::memset(&(sock.sin_zero), 0, sizeof(sock.sin_zero));

    return socket_id;
}

SocketHandler::~SocketHandler()
{
    close(this->sockfd);
}