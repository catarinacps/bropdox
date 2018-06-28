#include "server/Server.hpp"

Server::Server(port_t port_param)
    : port(port_param)
    , sock_handler(port_param)
    , port_manager(port)
{
}

bool Server::listen()
{
    auto data = this->sock_handler.wait_packet(sizeof(bdu::handshake_t));
    if (!data) {
        return false;
    }

    auto hand = bdu::convert_to_handshake(data.get());
    auto address = this->sock_handler.get_last_address();

    this->log(hand->userid, "New handshake received, creating receiver thread...");
    std::thread request_thread(&Server::treat_client_request, this, std::move(hand), address);

    if (!request_thread.joinable()) {
        this->log("-", "Failed to create new thread...");
        return false;
    }

    request_thread.detach();

    return true;
}

void Server::treat_client_request(std::unique_ptr<bdu::handshake_t> hand, sockaddr_in const client_addr)
{
    //TODO:
    // - check package (checksum) (is it really necessary?)

    if (hand->device == 0) {
        if (hand->req_type == bdu::req::login) {
            auto const reserved_port = this->port_manager.reserve_port();
            auto device = this->login_manager.login(hand->userid, client_addr, reserved_port);
            this->log(hand->userid, "Created a RH");

            bdu::syn_t syn(true, reserved_port, device);
            this->sock_handler.send_packet(&syn, sizeof(bdu::syn_t), client_addr);
            this->log(hand->userid, "Client now logged in");

            return;
        } else {
            bdu::syn_t syn(false, 0, 0);
            this->sock_handler.send_packet(&syn, sizeof(bdu::syn_t), client_addr);
            this->log(hand->userid, "Client not logged in1");

            return;
        }
    }

    auto& user = this->login_manager.get_client_data(hand->userid, hand->device);
    if (!user.initialized) {
        bdu::syn_t syn(false, 0, 0);
        this->sock_handler.send_packet(&syn, sizeof(bdu::syn_t), client_addr);
        this->log(hand->userid, "Client not logged in2");

        return;
    }

    bdu::syn_t syn(true, user.port, user.handler.get_device());
    this->sock_handler.send_packet(&syn, sizeof(bdu::syn_t), client_addr);
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
