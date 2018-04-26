#include <Server.hpp>

Server::Server() : sock_handler(SocketHandler(ADDR))
{
    // TODO: Construct a list of existing persistent clients
}

int Server::wait_client_request(int& desc)
{
    pid_t id;
    data_buffer_t* data;

    data = this->sock_handler.wait_packet(sizeof(handshake_t));
    if(data == NULL) {
        return 0;
    }

    // ! Probably the following will change as soon as I implement the request handler class...
    printf("=> New handshake received, forking receiver process...\n");
    id = fork();
    if (id == 0) {
        this->treat_client_request();

        exit(0);
    }

    return (int)id;
}

void Server::treat_client_request()
{
    int n, f_size;
    ack_t ack;
    bool pack_ok;
    handshake_t hand;
    data_buffer_t buffer;

    // TODO:
    // - check package (checksum) (is it really necessary?)
    pack_ok = true;

    if (!pack_ok) {
        printf("Bad request/handshake, turning down connection...\n");
        return;
    }

    buffer.resize(sizeof(handshake_t));
    convert_to_handshake(hand, buffer);

    // TODO:
    // - init client info/folder if necessary

    switch (hand.req_type) {
    case req::sync: {
        this->sync_server();
    } break;
    case req::send: {
        this->send_file(hand.file.name);
    } break;
    case req::receive: {
        this->receive_file(hand.file.name);
    } break;
    default:
        printf("Something went wrong...\n");
    }
}

void Server::send_file(char* file)
{
    //
}