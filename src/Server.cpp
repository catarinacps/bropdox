#include "../include/Server.hpp"

Server::Server()
    : sock_handler(SocketHandler(ADDR))
{
    // TODO: Construct a list of existing persistent clients
}

int Server::wait_client_request(int& desc)
{
    pid_t id;
    data_buffer_t* data;
    pthread_t new_thread;
    arg_thread_t* arguments;

    data = this->sock_handler.wait_packet(sizeof(handshake_t));
    if (data == NULL) {
        return 0;
    }

    // Allocates the arguments struct to the pthread_create call
    arguments = (arg_thread_t*) malloc(sizeof(arg_thread_t));
    arguments->context = this;
    arguments->hand_package = data;

    printf("=> New handshake received, forking receiver process...\n");
    if (pthread_create(&new_thread, NULL, &Server::treat_helper, arguments)) {
        //ERRO
        return -1;
    }
    //TODO: guardar descritor da thread sei la e fazer mais alguma coisa?

    return (int)id;
}

void* Server::treat_client_request(data_buffer_t* package)
{
    int n, f_size;
    ack_t ack;
    bool pack_ok;
    handshake_t hand;
    RequestHandler* rh;

    // TODO:
    // - check package (checksum) (is it really necessary?)
    pack_ok = true;

    if (!pack_ok) {
        printf("Bad request/handshake, turning down connection...\n");
        pthread_exit(NULL);
    }

    // Converts the received byte-array to a handshake struct
    convert_to_handshake(hand, *package);

    // Checks if the userid already has a declared RequestHandler
    if (this->user_list.count(hand.userid)) {
        // Declares on the heap a new Request Handler for the users request using the userid as the
        // socket address/path
        rh = new RequestHandler(hand.userid);
    } else {
        // Just accesses the poiter to the already declared class
        rh = this->user_list[hand.userid];
    }

    // TODO:
    // - init client info/folder if necessary
    // not here though kkkj

    switch (hand.req_type) {
    case req::sync: {
        rh->sync_server();
    } break;
    case req::send: {
        rh->send_file(hand.file.name);
    } break;
    case req::receive: {
        rh->receive_file(hand.file.name);
    } break;
    default:
        printf("Something went wrong...\n");
    }

    pthread_exit(NULL);
}

void* Server::treat_helper(void* arg)
{
    return (((arg_thread_t*)arg)->context)->treat_client_request(((arg_thread_t*)arg)->hand_package);
}