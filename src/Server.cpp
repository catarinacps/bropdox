#include "../include/Server.hpp"

Server::Server(in_port_t port_param)
    : sock_handler(port_param)
    , port(port_param)
    , port_counter(MAXPORT - port)
{
}

bool Server::listen()
{
    pthread_t new_thread;
    int ret_pcreate;

    auto data = this->sock_handler.wait_packet(sizeof(handshake_t));
    if (!data) {
        return false;
    }

    printf("=> New handshake received, creating receiver thread...\n");
    auto hand = convert_to_handshake(data.get());
    std::thread request_thread(&Server::treat_client_request, this, std::move(hand));

    if (!request_thread.joinable()) {
        printf("Failed to create new thread...");
        return false;
    } else {
        request_thread.detach();
    }

    return true;
}

void Server::treat_client_request(std::unique_ptr<handshake_t> hand)
{
    bool pack_ok, req_handl_ok;
    std::string file_name;
    RequestHandler* rh;

    // TODO:
    // - check package (checksum) (is it really necessary?)
    pack_ok = true;

    if (!pack_ok) {
        this->log(hand->userid, "Bad request/handshake");
        return;
    }

    std::pair<client_data_t, unsigned short> client_info;

    try {
        client_info = this->login(hand->userid, hand->device);
    } catch (std::invalid_argument const& e) {
        // Invalid device
        std::cerr << e.what() << '\n';

        syn_t syn(false, 0, 0);
        this->sock_handler.send_packet(&syn, sizeof(syn_t));
        this->log(hand->userid, "Invalid device argument in handshake");

        return;
    } catch (std::domain_error const& e) {
        // No device left
        std::cerr << e.what() << '\n';

        syn_t syn(false, 0, 0);
        this->sock_handler.send_packet(&syn, sizeof(syn_t));
        this->log(hand->userid, "Too many devices logged in");

        return;
    }

    this->log(hand->userid, "Declared a new RequestHandler for the request");

    // Sends to the client a syn packet containing a bool and the new port he is supposed to use
    syn_t syn(true, client_info.first.second, client_info.second);
    this->sock_handler.send_packet(&syn, sizeof(syn_t));
    this->log(hand->userid, "Sent a SYN to the client");

    if (hand->req_type == req::login) {
        this->log(hand->userid, "Client logged in");
        return;
    }

    // Calls the RequestHandler to handle the client's request
    this->log(hand->userid, "Calling the created RequestHandler...");
    req_handl_ok = rh->handle_request(hand->req_type);

    if (!req_handl_ok) {
        this->log(hand->userid, "Communication with RequestHandler failed");
        return;
    }

    return;
}

std::pair<client_data_t, unsigned short> Server::login(std::string const& user_id, unsigned short int device)
{
    if (device > MAX_CONCURRENT_USERS) {
        throw std::invalid_argument("Server::login : invalid device argument");
    }

    if (device == 0) {
        device = this->get_device(user_id);
        if (device == 0) {
            throw std::domain_error("Server::login : too many devices logged in");
        }

        // Reserves a new port to the new RequestHandler
        auto const new_port = this->get_next_port();
        auto const client_addr = this->sock_handler.get_last_peeraddr();

        auto rh = std::make_unique<RequestHandler>(client_addr, new_port, user_id);

        auto client = std::make_pair(std::move(rh), new_port);

        this->users.at(user_id).at(device - 1) = std::move(client);
    }

    return std::make_pair(this->users[user_id].at(device - 1), device);
}

bool Server::logout(std::string const& user_id, unsigned short int const& device)
{
    if (device > MAX_CONCURRENT_USERS || device == 0) {
        throw std::invalid_argument("Server::login : invalid device argument");
    }

    this->users.at(user_id).at(device).first.reset();
    this->users.at(user_id).at(device).second = 0;

    return true;
}

unsigned short int Server::get_device(std::string const& user_id) const noexcept
{
    auto i = 0;
    for (auto const& item : this->users.at(user_id)) {
        if (item.second == 0) {
            return i;
        }
        i++;
    }

    return 0;
}

unsigned int Server::get_next_port() noexcept
{
    auto i = 1;
    for (auto const& occupied : this->port_counter) {
        if (!occupied) {
            return this->port + i;
        }
        i++;
    }

    return 0;
}

void Server::log(char const* userid, char const* message) const noexcept
{
    printf("Server [UID: %s]: %s\n", userid, message);
}
