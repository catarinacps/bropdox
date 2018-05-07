#include "../include/Server.hpp"

Server::Server()
{
    this->sock_handler = new SocketHandler(ADDR);
    // TODO: Construct a list of existing persistent clients
}

int Server::wait_client_request(int& desc)
{
    pid_t id;
    data_buffer_t* data;
    pthread_t new_thread;
    arg_thread_t* arguments;

    data = this->sock_handler->wait_packet(sizeof(handshake_t));
    if (data == NULL) {
        return 0;
    }

    // Allocates the arguments struct to the pthread_create call
    arguments = new arg_thread_t;
    arguments->context = this;
    arguments->hand_package = data;

    printf("=> New handshake received, forking receiver process...\n");
    if (pthread_create(&new_thread, NULL, &Server::treat_helper, arguments)) {
        //ERRO
        return -1;
    }
    //TODO: Maybe store the thread descriptor?

    return (int)id;
}

void* Server::treat_client_request(data_buffer_t* package)
{
    int n, f_size;
    ack_t ack;
    bool pack_ok;
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
    hand = convert_to_handshake(*package);
    delete package;

    // Checks if the userid already has a declared RequestHandler
    if (this->user_list.count(hand->userid)) {
        // Declares on the heap a new Request Handler for the users request using the userid as the
        // socket address/path
        rh = new RequestHandler(hand->userid);
        this->user_list[hand->userid] = rh;
    } else {
        // Just accesses the poiter to the already declared class
        rh = this->user_list[hand->userid];
    }

    // TODO:
    // - init client info/folder if necessary
    // not here though kkkj

    switch (hand->req_type) {
    case req::sync: {
        rh->sync_server();
    } break;
    case req::send: {
        rh->send_file(hand->file.name);
    } break;
    case req::receive: {
        rh->receive_file(hand->file.name);
    } break;
    default:
        printf("Something went wrong...\n");
    }

    pthread_exit((void*)0);
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