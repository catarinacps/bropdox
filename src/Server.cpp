#include "../include/Server.hpp"

Server::Server(in_port_t port_param)
{
    this->sock_handler = new SocketHandler(port_param);
    this->port = port_param;
    this->port_counter = port_param + 1;
}

bool Server::listen()
{
    pthread_t new_thread;
    int ret_pcreate;

    auto data = this->sock_handler->wait_packet(sizeof(handshake_t));
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

    // Reserves a new port to the new RequestHandler
    in_port_t new_port = this->get_next_port();

    // Checks if the userid already has a declared RequestHandler
    rh = new RequestHandler(this->sock_handler->get_last_peeraddr(), new_port, hand->userid);
    this->log(hand->userid, "Declared a new RequestHandler for the request");

    // Sends to the client a syn packet containing a bool and the new port he is supposed to use
    syn_t syn(true, new_port);
    this->sock_handler->send_packet(&syn, sizeof(syn_t));
    this->log(hand->userid, "Sent a SYN to the client");

    //TODO: Separate the following behaviour to another method
    //FIXME: Fix the memory leak on new client_data_t
    if (this->user_list.count(hand->userid) > 0) {
        // Declares on the heap a new Request Handler for the user's request
        if (this->user_list[hand->userid]->handlers[0] == nullptr) {
            this->user_list[hand->userid]->handlers[0] = rh;
            this->user_list[hand->userid]->ports[0] = new_port;
        } else {
            this->user_list[hand->userid]->handlers[1] = rh;
            this->user_list[hand->userid]->ports[1] = new_port;
        }
    } else {
        // Declares a new Request Handler for the new user's device
        this->user_list[hand->userid] = new client_data_t;
        this->user_list[hand->userid]->handlers[0] = rh;
        this->user_list[hand->userid]->ports[0] = new_port;
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

int Server::get_next_port()
{
    if (this->port_counter == MAXPORT) {
        this->port_counter = this->port + 1;
        return this->port_counter;
    }
    this->port_counter++;
    return this->port_counter;
}

bool Server::logout_client(int device, std::string user_id)
{
    if (device <= 2 && this->user_list[user_id]->handlers[device] != nullptr) {
        delete this->user_list[user_id]->handlers[device];
        this->user_list[user_id]->ports[device] = 0;

        return true;
    }

    return false;
}

void Server::log(char const* userid, char const* message) const
{
    printf("Server [UID: %s]: %s\n", userid, message);
}

Server::~Server()
{
    delete this->sock_handler;
}
