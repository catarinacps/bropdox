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
        perror("setsockopt (timeout)");
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

bool Socket::cork() noexcept
{
    int one = 1;

    auto ret = setsockopt(this->sock_fd, IPPROTO_UDP, UDP_CORK, &one, sizeof(one));
    if (ret == -1) {
        perror("setsockopt (cork)");
        return false;
    }

    this->is_corked = true;
    return true;
}

bool Socket::uncork() noexcept
{
    int zero = 0;

    auto ret = setsockopt(this->sock_fd, IPPROTO_UDP, UDP_CORK, &zero, sizeof(zero));
    if (ret == -1) {
        perror("setsockopt (uncork)");
        return false;
    }

    this->is_corked = false;
    return true;
}
}