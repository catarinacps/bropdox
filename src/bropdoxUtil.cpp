#include <bropdoxUtil.hpp>

int init_unix_socket(struct sockaddr_un& sock, char const* path)
{
    int socket_id;
    if ((socket_id = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        printf("Error while initializing the socket\n");
        return -1;
    }

    sock.sun_family = AF_UNIX;
    memcpy(sock.sun_path, path, _PC_PATH_MAX);

    return socket_id;
}

void convert_to_handshake(handshake_t& hand, data_buffer_t& data)
{
    memcpy(&hand, data.data(), sizeof(handshake_t));
}

void convert_to_data(data_buffer_t& data, packet_t& packet)
{
    memcpy(data.data(), &packet, sizeof(packet_t));
}

void convert_to_data(data_buffer_t& data, handshake_t& hand)
{
    memcpy(data.data(), &hand, sizeof(handshake_t));
}

void convert_to_data(data_buffer_t& data, ack_t& ack)
{
    memcpy(data.data(), &ack, sizeof(ack_t));
}