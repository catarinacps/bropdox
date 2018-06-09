#include "../include/Server.hpp"

Server::Server(port_t port_param)
    : sock_handler(port_param)
    , port(port_param)
    , port_manager(port)
{
}

bool Server::listen()
{
    auto data = this->sock_handler.wait_packet(sizeof(handshake_t));
    if (!data) {
        return false;
    }

    printf("=> New handshake received, creating receiver thread...\n");
    auto hand = convert_to_handshake(data.get());
    auto address = this->sock_handler.get_last_address();
    std::thread request_thread(&Server::treat_client_request, this, std::move(hand), address);

    if (!request_thread.joinable()) {
        printf("Failed to create new thread...");
        return false;
    }

    request_thread.detach();

    return true;
}

void Server::treat_client_request(std::unique_ptr<handshake_t> hand, sockaddr_in const client_addr)
{
    // TODO:
    // - check package (checksum) (is it really necessary?)

    if (hand->device == 0) {
        if (hand->req_type == req::login) {
            auto const new_port = this->port_manager.reserve_port();
            auto device = this->login_manager.login(hand->userid, client_addr, new_port);

            syn_t syn(true, new_port, device);
            this->sock_handler.send_packet(&syn, sizeof(syn_t));
            this->log(hand->userid, "Client now logged in");

            return;
        } else {
            syn_t syn(false, 0, 0);
            this->sock_handler.send_packet(&syn, sizeof(syn_t));
            this->log(hand->userid, "Client not logged in");

            return;
        }
    }

    auto& user = this->login_manager.get_client_data(hand->userid, hand->device);

    if (!user.initialized) {
        syn_t syn(false, 0, 0);
        this->sock_handler.send_packet(&syn, sizeof(syn_t));
        this->log(hand->userid, "Client not logged in");

        return;
    }

    syn_t syn(true, user.port, user.handler.get_device());
    this->sock_handler.send_packet(&syn, sizeof(syn_t));
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
    printf("Server [UID: %s]: %s\n", userid, message);
}
