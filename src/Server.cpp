#include "../include/Server.hpp"

Server::Server(in_port_t port_param)
    : sock_handler(port_param)
    , port(port_param)
    , port_counter(MAXPORT - port)
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
    bool pack_ok;
    bool req_handl_ok;

    unsigned short int client_device = 0;

    std::string file_name;

    // TODO:
    // - check package (checksum) (is it really necessary?)
    pack_ok = true;

    if (!pack_ok) {
        this->log(hand->userid, "Bad request/handshake");
        return;
    }

    if (!this->verify_login(hand->userid, hand->device)) {
        if (hand->req_type == req::login) {
            client_device = this->treat_client_login(hand->userid);

            if (client_device != 0) {
                this->log(hand->userid, "Client logged in");
            }
        } else {
            syn_t syn(false, 0, 0);
            this->sock_handler.send_packet(&syn, sizeof(syn_t));
            this->log(hand->userid, "User not logged in");
        }
        
        return;
    } else {
        client_device = hand->device;
    }

    auto& client_info = this->users.at(hand->userid).at(client_device - 1);

    this->log(hand->userid, "Declared a new RequestHandler for the request");

    // Sends to the client a syn packet containing a bool and the new port he is supposed to use
    syn_t syn(true, client_info.port, client_info.handler.get_device());
    this->sock_handler.send_packet(&syn, sizeof(syn_t));

    this->log(hand->userid, "Sent a SYN to the client");

    if (hand->req_type == req::login) {
        this->log(hand->userid, "Client logged in");
        return;
    }

    // Calls the RequestHandler to handle the client's request
    this->log(hand->userid, "Calling the created RequestHandler...");
    req_handl_ok = client_info.handler.handle_request(hand->req_type);

    if (!req_handl_ok) {
        this->log(hand->userid, "Communication with RequestHandler failed");
        return;
    }

    return;
}

bool Server::verify_login(std::string const& user_id, unsigned short int device) const noexcept
{
    if (device == 0 || device > MAX_CONCURRENT_USERS) {
        return false;
    }

    auto& login = this->users.at(user_id).at(device - 1);

    return login.initialized;
}

unsigned short int Server::treat_client_login(std::string const& user_id)
{
    unsigned short int client_device = 0;

    try {
        client_device = this->login(user_id);
    } catch (std::invalid_argument const& e) {
        // Invalid device
        std::cerr << e.what() << '\n';

        syn_t syn(false, 0, 0);
        this->sock_handler.send_packet(&syn, sizeof(syn_t));
        this->log(user_id.c_str(), "Invalid device argument in handshake");

        return 0;
    } catch (std::domain_error const& e) {
        // No device left
        std::cerr << e.what() << '\n';

        syn_t syn(false, 0, 0);
        this->sock_handler.send_packet(&syn, sizeof(syn_t));
        this->log(user_id.c_str(), "Too many devices logged in");

        return 0;
    }

    return client_device;
}

unsigned short int Server::login(std::string const& user_id)
{
    unsigned short int device;

    device = this->get_device(user_id);
    if (device == 0) {
        throw std::domain_error("Server::login : too many devices logged in");
    }

    // Reserves a new port to the new RequestHandler
    auto const new_port = this->reserve_next_port();
    auto const client_addr = this->sock_handler.get_last_peeraddr();

    RequestHandler rh(client_addr, new_port, device, user_id);

    this->users.at(user_id).at(device - 1) = client_data_t(std::move(rh), new_port);

    return device;
}

bool Server::logout(std::string const& user_id, unsigned short int device)
{
    if (device > MAX_CONCURRENT_USERS || device == 0) {
        throw std::invalid_argument("Server::login : invalid device argument");
    }

    this->users.at(user_id).at(device) = client_data_t();

    return true;
}

unsigned short int Server::get_device(std::string const& user_id) const noexcept
{
    auto i = 0;
    for (auto const& item : this->users.at(user_id)) {
        if (!item.initialized) {
            return i;
        }
        i++;
    }

    return 0;
}

void Server::log(char const* userid, char const* message) const noexcept
{
    printf("Server [UID: %s]: %s\n", userid, message);
}

unsigned int Server::reserve_next_port() noexcept
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