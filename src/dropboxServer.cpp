#include <dropboxServer.hpp>

#define PORT 4000

int main()
{
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buf[256];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("ERROR opening socket");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr)) < 0) {
        printf("ERROR on binding");
    }

    clilen = sizeof(struct sockaddr_in);

    while (true) {
        /* receive from socket */
        n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr*)&cli_addr, &clilen);
        if (n < 0) {
            printf("ERROR on recvfrom");
        }
        printf("Received a datagram: %s\n", buf);

        /* send to socket */
        n = sendto(sockfd, "Got your message\n", 18, 0, (struct sockaddr*)&cli_addr, sizeof(struct sockaddr));
        if (n < 0) {
            printf("ERROR on sendto");
        }
        memset(buf, 0, sizeof(buf));
    }

    close(sockfd);
    return 0;
}

/******************************************************************************
 * Server Class method and constructor definitions
 */

Server::Server()
{
    this->buffer.resize(HANDSIZE);
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