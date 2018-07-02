#include "server/ReplicaManager.hpp"

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

    switch (argc) {
    case 1:
    case 2: {
        printf("Incorrect parameter usage, please refer to the following model:\n");
        printf("\t./server <port> <mode> [-v]\n");
        printf("where mode is either '--primary' or '--backup'\n\n");

        return -1;
    }
    case 3: {
        std::string mode(argv[2]);

        if (mode == "--primary") {
            // auto replica = ReplicaManager::make_primary(atoi(argv[1]), false);

            // replica.run();
        } else if (mode == "--backup") {
            // auto replica = ReplicaManager::make_backup(atoi(argv[1]), false);

            // replica.run();
        } else {
            printf("Incorrect parameter usage, please refer to the following model:\n");
            printf("\t./server <port> <mode> [-v]\n");
            printf("where mode is either '--primary' or '--backup'\n\n");

            return -1;
        }
    }
    case 4: {
        std::string mode(argv[2]);
        std::string verb(argv[3]);
        bool verbose = false;
        
        if (verb == "-v") {
            verbose = true;
        }

        if (mode == "--primary") {
            // auto replica = ReplicaManager::make_primary(atoi(argv[1]), verbose);

            // replica.run();
        } else if (mode == "--backup") {
            // auto replica = ReplicaManager::make_backup(atoi(argv[1]), verbose);

            // replica.run();
        } else {
            printf("Incorrect parameter usage, please refer to the following model:\n");
            printf("\t./server <port> <mode> [-v]\n");
            printf("where mode is either '--primary' or '--backup'\n\n");

            return -1;
        }
    }
    default: {
        printf("Incorrect parameter usage, please refer to the following model:\n");
        printf("\t./server <port> <mode> [-v]\n");
        printf("where mode is either '--primary' or '--backup'\n\n");

        return -1;
    }
    }

    return 0;
}