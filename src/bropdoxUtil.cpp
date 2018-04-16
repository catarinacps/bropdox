#include <bropdoxUtil.hpp>

int init_unix_socket(struct sockaddr_un* sock, char const* path)
{
    int socket_id;
    if ((socket_id = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        printf("Error while initializing the socket\n");
        return -1;
    }

    sock->sun_family = AF_UNIX;
    memcpy(sock->sun_path, path, _PC_PATH_MAX);

    return socket_id;
}

void convert_to_handshake(handshake_t* hand, con_buffer_t* buffer)
{
    memcpy(hand, (*buffer).data(), sizeof(handshake_t));
}

void convert_to_data(con_buffer_t* data, packet_t* packet)
{
    memcpy((*data).data(), packet, sizeof(packet_t));
}