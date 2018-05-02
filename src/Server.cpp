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

    data = this->sock_handler.wait_packet(sizeof(handshake_t));
    if (data == NULL) {
        return 0;
    }

    // ! Probably the following will change as soon as I implement the request handler class...
    printf("=> New handshake received, forking receiver process...\n");

    if (pthread_create(&new_thread, NULL, &Server::treat_helper, this)) {
        //ERRO
        return -1;
    }
    //TODO: guardar descritor da thread sei la e fazer mais alguma coisa?

    return (int)id;
}

void* Server::treat_client_request()
{
    int n, f_size;
    ack_t ack;
    bool pack_ok;
    handshake_t hand;
    data_buffer_t buffer;
    RequestHandler* rh;

    // TODO:
    // - check package (checksum) (is it really necessary?)
    pack_ok = true;

    if (!pack_ok) {
        printf("Bad request/handshake, turning down connection...\n");
        return;
    }

    // Resizes the buffer and converts the received byte-array to a handshake struct
    buffer.resize(sizeof(handshake_t));
    convert_to_handshake(hand, buffer);

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

    pthread_exit(0);
}

void* Server::treat_helper(void* context)
{
    return ((Server*)context)->treat_client_request();
}