#include "networking/ActiveSocket.hpp"

namespace networking {

ActiveSocket::ActiveSocket(port_t peer_port, const std::string& peer_addr)
    : is_corked(false)
{
    this->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (this->sock_fd == -1) {
        perror("socket");
        throw bdu::socket_bad_create();
    }

    auto opt = this->init_peer_addr(peer_port, peer_addr.c_str());
    if (!opt) {
        //TODO: log
        // error on addr init
        throw bdu::socket_bad_create();
    }

    this->peer_address = opt.value();

    if (!this->connect_to(this->peer_address)) {
        //TODO: log
        // error on connect
        throw bdu::socket_bad_create();
    }

    if (!this->set_timeout(TIMEOUT_SEC, TIMEOUT_USEC)) {
        //TODO: log
        // error on setsockopt
        throw bdu::socket_bad_create();
    }

    auto address = this->get_own_address();
    if (!address) {
        throw bdu::socket_bad_create();
    }

    this->own_address = address.value();
}

ActiveSocket::ActiveSocket(const sockaddr_in& peer)
    : is_corked(false)
{
    this->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (this->sock_fd == -1) {
        perror("socket");
        throw bdu::socket_bad_create();
    }

    this->peer_address = peer;

    if (!this->connect_to(this->peer_address)) {
        //TODO: log
        // error on connect
        throw bdu::socket_bad_create();
    }

    if (!this->set_timeout(TIMEOUT_SEC, TIMEOUT_USEC)) {
        //TODO: log
        // error on setsockopt
        throw bdu::socket_bad_create();
    }

    auto address = this->get_own_address();
    if (!address) {
        throw bdu::socket_bad_create();
    }

    this->own_address = address.value();
}

ActiveSocket::ActiveSocket(ActiveSocket&& move)
    : sock_fd(move.sock_fd)
    , is_corked(move.is_corked)
    , own_address(move.own_address)
    , peer_address(move.peer_address)
{
    move.sock_fd = -1;
}

ActiveSocket::~ActiveSocket()
{
    if (this->sock_fd > 0) {
        close(this->sock_fd);
    }
}

std::optional<sockaddr_in> ActiveSocket::init_peer_addr(port_t port, const char* addr)
{
    sockaddr_in peer_addr{};

    // memset(&peer_addr, 0, sizeof(sockaddr_in));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(port);
    auto ret = inet_pton(AF_INET, addr, &peer_addr.sin_addr);

    if (ret == 0) {
        //TODO: log
        // invalid character in address
        return {};
    } else if (ret == -1) {
        //TODO: log
        // invalid address family (??)
        perror("inet_pton");
        return {};
    }

    return peer_addr;
}

ssize_t ActiveSocket::send_file_block(int file_desc) const
{
    auto written_bytes = sendfile(this->sock_fd, file_desc, NULL, BUFFER_SIZE);

    if (written_bytes == -1) {
        perror("sendfile");
        return -1;
    }

    return written_bytes;
}

ssize_t ActiveSocket::recv_file_block(int file_desc) const
{
    byte_t buffer[BUFFER_SIZE];

    auto read_bytes = read(this->sock_fd, buffer, BUFFER_SIZE);

    if (read_bytes == -1) {
        perror("read");
        return -1;
    }

    auto written_bytes = write(file_desc, buffer, read_bytes);

    if (written_bytes == -1) {
        perror("write");
        return -1;
    } else if (written_bytes < read_bytes) {
        //TODO: log
        // unexpected short write
        return -1;
    }

    return written_bytes;
}

bool ActiveSocket::change_peer(const sockaddr_in& new_peer) noexcept
{
    auto success = this->connect_to(new_peer);

    if (success) {
        this->peer_address = new_peer;
    }

    return success;
}
}