#include <Server.hpp>

Server::Server()
{
    this->buffer.resize(sizeof(handshake_t));
    this->sockfd = init_unix_socket(&(this->server_address), ADDR);
    this->client_len = sizeof(struct sockaddr_un);

    if (bind(this->sockfd, (struct sockaddr*)&(this->server_address), sizeof(struct sockaddr)) < 0) {
        printf("Error while binding the socket, please try again...\n");
    }
}

int Server::wait_client_request(int* desc)
{
    pid_t id;

    *desc = recvfrom(this->sockfd, (void*)this->buffer.data(), sizeof(handshake_t), 0, (struct sockaddr*)&(this->client_address), &(this->client_len));
    if (*desc < 0) {
        printf("Error while receiving handshake...\n\n");
        return 0;
    }

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

    // TODO:
    // - check package (checksum) (is it really necessary?)
    pack_ok = true;

    if (!pack_ok) {
        printf("Bad request/handshake, turning down connection...\n");
        return;
    }

    convert_to_handshake(&hand, &(this->buffer));

    switch (hand.req_type) {
    case req::sync: {
        Server::sync_server();
    } break;
    case req::send: {
        Server::send_file(hand.file.name);
    } break;
    case req::receive: {
        Server::receive_file(hand.file.name);
    } break;
    default:
        printf("Something went wrong...\n");
    }
}

void Server::send_file(char* file)
{
    //
}