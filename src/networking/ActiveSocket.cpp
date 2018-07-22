#include "networking/ActiveSocket.hpp"

namespace networking {

ActiveSocket::ActiveSocket(port_t peer_port, const std::string& peer_addr)
{
    this->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (this->sock_fd == -1) {
        perror("socket");
        throw bdu::socket_bad_create();
    }

    if (!init_peer_addr(this->peer_address, peer_port, peer_addr.c_str())) {
        //TODO: log
        // error on addr init
        throw bdu::socket_bad_create();
    }

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
}

ActiveSocket::ActiveSocket(const sockaddr_in& peer)
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
}

ActiveSocket::ActiveSocket(ActiveSocket&& move)
{
    this->sock_fd = move.sock_fd;
    this->own_address = move.own_address;
    this->peer_address = move.peer_address;

    move.sock_fd = -1;
}

ActiveSocket::~ActiveSocket()
{
    if (this->sock_fd > 0) {
        close(this->sock_fd);
    }
}

bool ActiveSocket::init_peer_addr(sockaddr_in& peer, port_t port, const char* addr)
{
    memset(&peer, 0, sizeof(sockaddr_in));
    peer.sin_family = AF_INET;
    peer.sin_port = htons(port);
    auto ret = inet_pton(AF_INET, addr, &peer.sin_addr);

    if (ret == 0) {
        //TODO: log
        // invalid character in address
        return false;
    } else if (ret == -1) {
        //TODO: log
        // invalid address family (??)
        perror("inet_pton");
        return false;
    }

    return true;
}

ssize_t ActiveSocket::send_file(int file_desc)
{
    ssize_t written_bytes = 0, bytes_sum = 0;

    auto current_offset = lseek(file_desc, 0, SEEK_CUR);

    while ((written_bytes = sendfile(this->sock_fd, file_desc, NULL, BUFFER_SIZE)) > 0) {
        bytes_sum += written_bytes;
    }

    if (written_bytes == -1) {
        perror("sendfile");
        lseek(file_desc, current_offset, SEEK_SET);
        return -1;
    }

    lseek(file_desc, current_offset, SEEK_SET);
    return bytes_sum;
}

ssize_t ActiveSocket::recv_file(int file_desc)
{
    byte_t buffer[BUFFER_SIZE];

    ssize_t read_bytes = 0, bytes_sum = 0;

    while ((read_bytes = read(this->sock_fd, buffer, BUFFER_SIZE)) > 0) {
        auto written_bytes = write(file_desc, buffer, read_bytes);

        if (written_bytes == -1) {
            perror("write");
            return -1;
        } else if (written_bytes < read_bytes) {
            //TODO: log
            // unexpected short write
            return -1;
        }

        bytes_sum += written_bytes;
    }

    if (read_bytes == -1) {
        perror("read");
        return -1;
    }

    return bytes_sum;
}
}