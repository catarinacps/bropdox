#include "../include/Server.hpp"

Server::Server(in_port_t port_param)
{
    this->sock_handler = new SocketHandler(port_param);
    this->port = port_param;
    this->port_counter = port_param + 1;

}

int Server::wait_client_request()
{
    data_buffer_t* data;
    pthread_t new_thread;
    arg_thread_t* arguments;
    int ret_pcreate;

    data = this->sock_handler->wait_packet(sizeof(handshake_t));
    if (data == NULL) {
        return 0;
    }

    // Allocates the arguments struct to the pthread_create call
    arguments = new arg_thread_t;
    arguments->context = this;
    arguments->hand_package = data;

    printf("=> New handshake received, forking receiver process...\n");
    if ((ret_pcreate = pthread_create(&new_thread, NULL, &Server::treat_helper, arguments))) {
        printf("Failed to create new thread...");
        return ret_pcreate;
    }
    //TODO: Maybe store the thread descriptor?

    return ret_pcreate;
}

void* Server::treat_client_request(data_buffer_t* package)
{
    bool pack_ok, req_handl_ok;
    std::string file_name;
    handshake_t* hand;
    RequestHandler* rh;

    // TODO:
    // - check package (checksum) (is it really necessary?)
    pack_ok = true;

    if (!pack_ok) {
        printf("Bad request/handshake, turning down connection...\n");
        pthread_exit((void*)-1);
    }

    // Converts the received byte-array to a handshake struct
    hand = convert_to_handshake(package);
    delete package;

    // Checks if the userid already has a declared RequestHandler
    rh = new RequestHandler(this->sock_handler->get_last_clientaddr(), this->get_next_port(), hand->userid);
    if (this->user_list.count(hand->userid) > 0) {
        // Declares on the heap a new Request Handler for the user's request
        if (this->user_list[hand->userid]->handlers[0] == nullptr) {
            this->user_list[hand->userid]->handlers[0] = rh;
            this->user_list[hand->userid]->ports[0] = this->get_next_port();
        } else {
            this->user_list[hand->userid]->handlers[1] = rh;
            this->user_list[hand->userid]->ports[1] = this->get_next_port();
        }
    } else {
        // Declares a new Request Handler for the new user's device
        this->user_list[hand->userid] = new client_data_t;
        this->user_list[hand->userid]->handlers[0] = rh;
        this->user_list[hand->userid]->ports[0] = this->get_next_port();
    }

    switch (hand->req_type) {
    case req::sync:
        req_handl_ok = rh->wait_request(hand->req_type);
        break;
    case req::send:
    case req::receive:
        req_handl_ok = rh->wait_request(hand->req_type, hand->file);
        break;
    default:
        printf("Something went wrong...\n");
        req_handl_ok = false;
    }

    if (!req_handl_ok) {
        printf("Communication with RequestHandler failed...");
        pthread_exit((void*)-1);
    }

    delete hand;
    pthread_exit((void*)0);
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

bool Server::deallocate_request_chandler(int device, std::string user_id)
{
    if (device <= 2 && this->user_list[user_id]->handlers[device] != nullptr) {
        delete this->user_list[user_id]->handlers[device];
        this->user_list[user_id]->ports[device] = 0;

        return true;
    }

    return false;
}

void* Server::treat_helper(void* arg)
{
    // This static class method helps the initialization of the helper thread
    // by calling the treat_client_request method using the parameter arg,
    // that is, essentially, thread_helper_t, which contains the object context
    // and the package argument to the function.
    return (((arg_thread_t*)arg)->context)->treat_client_request(((arg_thread_t*)arg)->hand_package);
}

Server::~Server()
{
    delete this->sock_handler;
}
