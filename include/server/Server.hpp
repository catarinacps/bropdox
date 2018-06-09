#pragma once

#include "server/LoginManager.hpp"
#include "server/PortManager.hpp"
#include "server/RequestHandler.hpp"
#include "util/SocketHandler.hpp"

#include <thread>

class Server {
    port_t const port;

    SocketHandler mutable sock_handler;
    LoginManager login_manager;
    PortManager port_manager;

public:
    /**
     * Sets the server to wait the next handshake package from a client.
     * 
     * @return success or failure on receiving said handshake
     */
    bool listen();

private:
    /**
     * Treats the client handshake.
     * 
     * Gets called on the creation of a treater thread 
     * 
     * @param package the handshake
     */
    void treat_client_request(std::unique_ptr<handshake_t> hand, sockaddr_in const client_addr);

    /**
     * Logs a message to the console.
     */
    void log(char const* userid, char const* message) const noexcept;

public:
    Server(port_t port);
};
