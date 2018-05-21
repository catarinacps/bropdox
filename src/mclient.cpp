#include "../include/Client.hpp"

int main(int argc, char* argv[])
{
    // int sockfd, n;
    // unsigned int length;
    // struct sockaddr_in serv_addr, from;
    // struct hostent* server;

    // char buffer[256];
    // if (argc < 2) {
    //     fprintf(stderr, "usage %s hostname\n", argv[0]);
    //     exit(0);
    // }

    // server = gethostbyname(argv[1]);
    // if (server == NULL) {
    //     fprintf(stderr, "ERROR, no such host\n");
    //     exit(0);
    // }

    // if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    //     printf("ERROR opening socket");
    // }

    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_port = htons(PORT);
    // serv_addr.sin_addr = *((struct in_addr*)server->h_addr);
    // bzero(&(serv_addr.sin_zero), 8);

    // printf("Enter the message: ");
    // bzero(buffer, 256);
    // fgets(buffer, 256, stdin);

    // n = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in));
    // if (n < 0) {
    //     printf("ERROR sendto");
    // }

    // length = sizeof(struct sockaddr_in);
    // n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr*)&from, &length);
    // if (n < 0) {
    //     printf("ERROR recvfrom");
    // }

    // printf("Got an ack: %s\n", buffer);

    // close(sockfd);
    // return 0;

    if (argc != 2) {
        printf("Incorrect parameter usage, please refer to the following model:\n");
        printf("./mclient <userid>\n\n");

        return -1;
    }

    std::strcat(argv[1], "\0");

    Client client(argv[1]);

    client.command_line_interface();

    return 0;
}