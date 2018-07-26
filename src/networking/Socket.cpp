#include "networking/Socket.hpp"

namespace networking {

bool Socket::bind_to(const sockaddr_in& address) const noexcept
{
    auto ret = bind(this->sock_fd, (sockaddr*)&address, sizeof(sockaddr_in));
    if (ret == -1) {
        perror("bind");
        return false;
    }

    return true;
}

bool Socket::connect_to(const sockaddr_in& address) const noexcept
{
    auto ret = connect(this->sock_fd, (sockaddr*)&address, sizeof(sockaddr_in));
    if (ret == -1) {
        perror("connect");
        return false;
    }

    return true;
}

bool Socket::set_timeout(uint16_t sec, uint16_t microsec) const noexcept
{
    timeval timeout = { sec, microsec };

    auto ret = setsockopt(this->sock_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeval));
    if (ret == -1) {
        perror("timeout");
        return false;
    }

    return true;
}

std::optional<sockaddr_in> Socket::get_own_address() const noexcept
{
    sockaddr_in address{};
    socklen_t length = sizeof(sockaddr_in);

    auto ret = getsockname(this->sock_fd, (sockaddr*)&address, &length);
    if (ret == -1) {
        perror("getsockname");
        return {};
    } else if (length < sizeof(sockaddr_in)) {
        //TODO: log
        // less bytes written on own_address than expected
        return {};
    }

    return address;
}
}