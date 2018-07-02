#pragma once

#include "server/LoginManager.hpp"
#include "server/PortManager.hpp"
#include "server/RequestHandler.hpp"

#include "helpers/SocketHandler.hpp"

#include "util/Definitions.hpp"

#include <thread>
#include <memory>

class Server {
    port_t const port;

    SocketHandler mutable sock_handler;
    LoginManager login_manager;
    PortManager port_manager;

    bool listening = false;
    bool verbose;

public:
    /**
     * Sets the server to wait the next handshake package from a client.
     */
    void listen();

    /**
     * Stops listening for handshakes.
     */
    void stop();

    //TODO: Add a "update" or "refresh" method to sync the changes with the primary server.

private:
    /**
     * Treats the client handshake.
     * 
     * Gets called on the creation of a treater thread 
     * 
     * @param package the handshake
     */
    void treat_client_request(std::unique_ptr<bdu::handshake_t> hand, sockaddr_in const client_addr);

    /**
     * Logs a message to the console.
     */
    void log(char const* userid, char const* message) const noexcept;

public:
    Server(port_t port, bool verbose);
};
