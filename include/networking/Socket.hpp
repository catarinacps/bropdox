#pragma once

#include "util/Exception.hpp"

#include <memory>
#include <optional>
#include <utility>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace networking {

class Socket {
public:
    template <typename T>
    std::pair<std::unique_ptr<T>, sockaddr_in> recv_data() const = delete;

    template <typename T>
    bool send_data(const T* data, const sockaddr_in& address) const = delete;

    /**
     * 
     */
    bool cork() noexcept;

    /**
     * 
     */
    bool uncork() noexcept;

protected:
    int sock_fd;
    bool is_corked;
    sockaddr_in own_address;

    /**
     * 
     */
    bool bind_to(const sockaddr_in& address) const noexcept;

    /**
     * 
     */
    bool set_timeout(uint16_t sec, uint16_t microsec) const noexcept;

    /**
     * 
     */
    bool connect_to(const sockaddr_in& address) const noexcept;

    /**
     * 
     */
    std::optional<sockaddr_in> get_own_address() const noexcept;
};
}