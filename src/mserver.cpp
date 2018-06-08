#include "../include/Server.hpp"

int main(int argc, char* argv[])
{
    // int sockfd, n;
    // socklen_t clilen;
    // struct sockaddr_in serv_addr, cli_addr;
    // char buf[256];

    // if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    //     printf("ERROR opening socket");
    // }

    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_port = htons(PORT);
    // serv_addr.sin_addr.s_addr = INADDR_ANY;
    // bzero(&(serv_addr.sin_zero), 8);

    // if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr)) < 0) {
    //     printf("ERROR on binding");
    // }

    // clilen = sizeof(struct sockaddr_in);

    // while (true) {
    //     /* receive from socket */
    //     n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr*)&cli_addr, &clilen);
    //     if (n < 0) {
    //         printf("ERROR on recvfrom");
    //     }
    //     printf("Received a datagram: %s\n", buf);

    //     /* send to socket */
    //     n = sendto(sockfd, "Got your message\n", 18, 0, (struct sockaddr*)&cli_addr, sizeof(struct sockaddr));
    //     if (n < 0) {
    //         printf("ERROR on sendto");
    //     }
    //     memset(buf, 0, sizeof(buf));
    // }

    // close(sockfd);
    // return 0;

    if (argc != 2) {
        printf("Incorrect parameter usage, please refer to the following model:\n");
        printf("./mserver <port>\n\n");

        return -1;
    }

    Server server = Server((port_t)atoi(argv[1]));
    printf("Server: Server is live\n");

    while (true) {
        if (server.listen()) {
            printf("Server: Treating a request in a new thread...\n");
        } else {
            printf("Server: Failed handshake attempt received...\n");
        }
    }
}