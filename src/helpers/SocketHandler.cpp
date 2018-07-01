#include "helpers/SocketHandler.hpp"

/******************************************************************************
 * Constructors
 */

SocketHandler::SocketHandler(port_t port, hostent* server)
    : sockfd(-1)
    , peer_len(sizeof(struct sockaddr_in))
{
    struct timeval timeout = { 0, TIMEOUT };

    this->sockfd = SocketHandler::init_client_socket(this->peer_address, port, server);

    /* if (setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval)) < 0) {
        this->log("Error while setting the timeout, please try again...");
        throw bdu::socket_bad_opt();
    } */
}

SocketHandler::SocketHandler(port_t port, sockaddr_in peer_address_p)
    : sockfd(-1)
    , peer_len(sizeof(struct sockaddr_in))
{
    struct timeval timeout = { 0, TIMEOUT };

    this->peer_address = peer_address_p;

    this->sockfd = SocketHandler::init_server_socket(this->handler_address, port);

    /* if (setsockopt(this->sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval)) < 0) {
        this->log("Error while setting the timeout, please try again...");
        throw bdu::socket_bad_opt();
    } */

    if (bind(this->sockfd, (struct sockaddr*)&(this->handler_address), sizeof(struct sockaddr)) < 0) {
        this->log("Error while binding the socket, please try again...");
        throw bdu::socket_bad_bind();
    }
}

SocketHandler::SocketHandler(port_t port)
    : sockfd(-1)
    , peer_len(sizeof(struct sockaddr_in))
{
    this->sockfd = SocketHandler::init_server_socket(this->handler_address, port);

    if (bind(sockfd, (struct sockaddr*)&(this->handler_address), sizeof(struct sockaddr)) < 0) {
        this->log("Error while binding the socket, please try again...");
        throw bdu::socket_bad_bind();
    }
}

SocketHandler::SocketHandler(SocketHandler&& move)
    : sockfd(move.sockfd)
    , peer_len(std::move(move.peer_len))
    , handler_address(std::move(move.handler_address))
    , peer_address(std::move(move.peer_address))
{
    move.sockfd = -1;
}

/******************************************************************************
 * Member Functions
 */

sockaddr_in SocketHandler::get_last_address() const noexcept
{
    return this->peer_address;
}

void SocketHandler::log(char const* message) const
{
    printf("SocketHandler [sockfd: %d]: %s\n", this->sockfd, message);
}

int SocketHandler::init_server_socket(struct sockaddr_in& sock, port_t port)
{
    int socket_id;
    if ((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Error while initializing the socket\n");
        throw bdu::socket_bad_create();
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = INADDR_ANY;
    std::memset(&(sock.sin_zero), '\0', sizeof(sock.sin_zero));

    return socket_id;
}

int SocketHandler::init_client_socket(struct sockaddr_in& sock, port_t port, hostent* server)
{
    int socket_id;
    if ((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Error while initializing the socket\n");
        throw bdu::socket_bad_create();
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr = *((struct in_addr*)server->h_addr);
    std::memset(&(sock.sin_zero), '\0', sizeof(sock.sin_zero));

    return socket_id;
}

SocketHandler& SocketHandler::operator=(SocketHandler&& move)
{
    if (this != &move) {
        if (this->sockfd > 0) {
            close(this->sockfd);
        }
        this->sockfd = move.sockfd;
        this->peer_len = move.peer_len;
        this->peer_address = move.peer_address;
        this->handler_address = move.handler_address;
        move.sockfd = -1;
        move.peer_len = 0;
        std::memset(&(move.peer_address), '\0', sizeof(struct sockaddr_in));
        std::memset(&(move.handler_address), '\0', sizeof(struct sockaddr_in));
    }

    return *this;
}

SocketHandler::~SocketHandler()
{
    if (this->sockfd > 0) {
        this->log("Goodbye cruel world!");
        close(this->sockfd);
    }
}
