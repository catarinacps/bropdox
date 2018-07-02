#include "server/Server.hpp"

Server::Server(port_t port_param, bool verbose_param)
    : port(port_param)
    , sock_handler(port_param)
    , port_manager(port)
    , verbose(verbose_param)
{
}

void Server::listen()
{
    this->listening = true;

    this->log("-", "Server is alive!");

    while (this->listening) {
        auto hand = this->sock_handler.wait_packet<bdu::handshake_t>();
        if (!hand) {
            continue;
        }

        auto address = this->sock_handler.get_last_address();

        this->log(hand->userid, "New handshake received, creating receiver thread...");
        std::thread request_thread(&Server::treat_client_request, this, std::move(hand), address);

        if (!request_thread.joinable()) {
            this->log("-", "Failed to create new thread...");
            continue;
        }

        request_thread.detach();
    }
}

void Server::stop()
{
    this->listening = false;
}

void Server::treat_client_request(std::unique_ptr<bdu::handshake_t> hand, sockaddr_in const client_addr)
{
    if (hand->device == 0) {
        if (hand->req_type == bdu::req::login) {
            auto const reserved_port = this->port_manager.reserve_port();
            auto device = this->login_manager.login(hand->userid, client_addr, reserved_port);
            this->log(hand->userid, "Created a RH");

            bdu::syn_t syn(true, reserved_port, device);
            this->sock_handler.send_packet(&syn, client_addr);
            this->log(hand->userid, "Client now logged in");

            return;
        } else {
            bdu::syn_t syn(false, 0, 0);
            this->sock_handler.send_packet(&syn, client_addr);
            this->log(hand->userid, "Client not logged in");

            return;
        }
    }

    auto& user = this->login_manager.get_client_data(hand->userid, hand->device);
    if (!user.initialized) {
        bdu::syn_t syn(false, 0, 0);
        this->sock_handler.send_packet(&syn, client_addr);
        this->log(hand->userid, "Fake device, client not logged in");

        return;
    }

    bdu::syn_t syn(true, user.port, user.handler.get_device());
    this->sock_handler.send_packet(&syn, client_addr);
    this->log(hand->userid, "Sent to the client a syn with the port and device");

    // Calls the RequestHandler to handle the client's request
    this->log(hand->userid, "Calling the created RequestHandler...");

    if (!user.handler.handle_request(hand->req_type)) {
        this->log(hand->userid, "Communication with RequestHandler failed");
        return;
    }

    return;
}

void Server::log(char const* userid, char const* message) const noexcept
{
    if (this->verbose) {
        printf("Server [UID: %s]: %s\n", userid, message);
    }
}
