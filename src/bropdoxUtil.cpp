#include "../include/bropdoxUtil.hpp"

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

handshake_t* convert_to_handshake(data_buffer_t& data)
{
    handshake_t* hand = new handshake_t;
    memcpy(hand, data.data(), sizeof(handshake_t));
    return hand;
}

data_buffer_t* convert_to_data(packet_t& packet)
{
    data_buffer_t* data = new data_buffer_t;
    memcpy(data->data(), &packet, sizeof(packet_t));
    return data;
}

data_buffer_t* convert_to_data(handshake_t& hand)
{
    data_buffer_t* data = new data_buffer_t;
    memcpy(data->data(), &hand, sizeof(handshake_t));
    return data;
}

data_buffer_t* convert_to_data(ack_t& ack)
{
    data_buffer_t* data = new data_buffer_t;
    memcpy(data->data(), &ack, sizeof(ack_t));
    return data;
}