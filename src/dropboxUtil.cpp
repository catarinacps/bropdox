#include <dropboxUtil.h>

int init_unix_socket(sockaddr_un* sock, int type, char* path)
{
    int socket_id;
    if ((socket_id = socket(AF_UNIX, type, 0)) == -1) {
        std::cout << "Error while initializing the socket" << std::endl;
        return -1;
    }

    sock->sun_family = type;
    memcpy(sock->sun_path, path, sizeof(path));

    return socket_id;
}