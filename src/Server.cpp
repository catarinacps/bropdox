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
    }
    
    request_thread.detach();

    return true;
}

void Server::treat_client_request(std::unique_ptr<handshake_t> hand)
{
    bool pack_ok;
    bool req_handl_ok;

    device_t client_device = 0;

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

            this->log(hand->userid, "Client logged in");

        } else {
            syn_t syn(false, 0, 0);
            this->sock_handler.send_packet(&syn, sizeof(syn_t));
            this->log(hand->userid, "User not logged in");
        }

        return;
    } else {
        client_device = hand->device;
    }

    this->m_map.lock();

    auto& client_info = this->users.at(hand->userid).at(client_device - 1);

    this->m_map.unlock();

    this->log(hand->userid, "Declared a new RequestHandler for the request");

    // Sends to the client a syn packet containing a bool and the new port he is supposed to use
    syn_t syn(true, client_info.port, client_info.handler.get_device());
    this->sock_handler.send_packet(&syn, sizeof(syn_t));

    this->log(hand->userid, "Sent a SYN to the client");

    // Calls the RequestHandler to handle the client's request
    this->log(hand->userid, "Calling the created RequestHandler...");
    req_handl_ok = client_info.handler.handle_request(hand->req_type);

    if (!req_handl_ok) {
        this->log(hand->userid, "Communication with RequestHandler failed");
        return;
    }

    return;
}

bool Server::verify_login(std::string const& user_id, device_t device) const noexcept
{
    if (device == 0 || device > MAX_CONCURRENT_USERS) {
        return false;
    }

    try {
        this->m_map.lock();

        auto& login = this->users.at(user_id).at(device - 1);

        this->m_map.unlock();

        return login.initialized;
    } catch (std::out_of_range const& e) {
        std::cerr << e.what() << '\n';

        this->m_map.unlock();
        return false;
    }
}

device_t Server::treat_client_login(std::string const& user_id)
{
    device_t client_device = 0;

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

device_t Server::login(std::string const& user_id)
{
    this->m_login.lock();

    auto const device = this->reserve_device(user_id);
    auto const new_port = this->reserve_port();
    auto const client_addr = this->sock_handler.pop_peer_address();

    RequestHandler rh(client_addr, new_port, device, user_id);

    this->m_login.unlock();

    this->m_map.lock();

    this->users.at(user_id).at(device - 1) = client_data_t(std::move(rh), new_port);

    this->m_map.unlock();

    return device;
}

bool Server::logout(std::string const& user_id, device_t device)
{
    if (device > MAX_CONCURRENT_USERS || device == 0) {
        throw std::invalid_argument("Server::login : invalid device argument");
    }

    this->m_map.lock();

    this->users.at(user_id).at(device) = client_data_t();

    this->m_map.unlock();
    return true;
}

device_t Server::reserve_device(std::string const& user_id)
{
    auto i = 0;
    for (auto& item : this->users.at(user_id)) {
        if (!item.initialized) {
            item.initialized = true;
            return i + 1;
        }
        i++;
    }

    throw std::domain_error("Server::login : too many devices logged in");
}

void Server::log(char const* userid, char const* message) const noexcept
{
    printf("Server [UID: %s]: %s\n", userid, message);
}

in_port_t Server::reserve_port() noexcept
{
    auto i = 1;

    for (auto&& occupied : this->port_counter) {
        if (!occupied) {
            occupied = true;

            return this->port + i;
        }
        i++;
    }

    return 0;
}