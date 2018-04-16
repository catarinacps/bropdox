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

void Server::treat_client_request(con_buffer_t* buf, struct sockaddr_un* cli_addr)
{
    int n;
    bool pack_ok;
    handshake_t hand;

    // @TODO
    // - check package (checksum) (is it really necessary?)
    pack_ok = true;

    if (!pack_ok) {
        printf("Bad request/handshake, turning down connection...\n");
        return;
    }

    n = sendto(this->sockfd, "t", 2, 0, (struct sockaddr*)cli_addr, sizeof(struct sockaddr));
    if (n < 0) {
        return;
        printf("Error while sending ack...\n");
    }

    this->client_address = *cli_addr;
    convert_to_handshake(&hand, buf);

    this->buffer.resize(sizeof(packet_t));

    switch (hand.req_type) {
    case req::sync:
        // Server::sync_server();
        break;
    case req::send:
        // Server::send_file();
        break;
    case req::receive:
        // Server::receive_file();
        break;
    default:
        printf("Something went wrong...\n");
    }
}