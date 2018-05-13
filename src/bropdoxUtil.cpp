#include "../include/bropdoxUtil.hpp"

int init_unix_socket(struct sockaddr_un& sock, char const* path)
{
    int socket_id;
    if ((socket_id = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        printf("Error while initializing the socket\n");
        return -1;
    }

    sock.sun_family = AF_UNIX;
    std::memcpy(sock.sun_path, path, _PC_PATH_MAX);

    return socket_id;
}

handshake_t* convert_to_handshake(data_buffer_t* data)
{
    handshake_t* hand = new handshake_t;
    std::memcpy(hand, data, sizeof(handshake_t));
    return hand;
}

ack_t* convert_to_ack(data_buffer_t* data)
{
    ack_t* ack = new ack_t;
    std::memcpy(ack, data, sizeof(ack_t));
    return ack;
}

packet_t* convert_to_packet(data_buffer_t* data)
{
    packet_t* packet = new packet_t;
    std::memcpy(packet, data, sizeof(packet_t));
    return packet;
}

file_info_list_t* convert_to_file_list(data_buffer_t* data)
{
    file_info_list_t* packet = new file_info_list_t;
    std::memcpy(packet, data, sizeof(file_info_list_t));
    return packet;
}

convert_helper_t convert_to_data(packet_t& packet)
{
    data_buffer_t* data = new data_buffer_t[sizeof(packet_t)];
    convert_helper_t helper;

    helper.pointer = data;
    helper.size = sizeof(data);

    std::memcpy(data, &packet, sizeof(packet_t));

    return helper;
}

convert_helper_t convert_to_data(packet_t const& packet)
{
    data_buffer_t* data = new data_buffer_t[sizeof(packet_t)];
    convert_helper_t helper;

    helper.pointer = data;
    helper.size = sizeof(data);

    std::memcpy(data, &packet, sizeof(packet_t));

    return helper;
}

convert_helper_t convert_to_data(handshake_t& hand)
{
    data_buffer_t* data = new data_buffer_t[sizeof(handshake_t)];
    convert_helper_t helper;

    helper.pointer = data;
    helper.size = sizeof(data);

    std::memcpy(data, &hand, sizeof(handshake_t));

    return helper;
}

convert_helper_t convert_to_data(ack_t& ack)
{
    data_buffer_t* data = new data_buffer_t[sizeof(ack_t)];
    convert_helper_t helper;

    helper.pointer = data;
    helper.size = sizeof(data);

    std::memcpy(data, &ack, sizeof(ack_t));

    return helper;
}

convert_helper_t convert_to_data(syn_t& syn)
{
    data_buffer_t* data = new data_buffer_t[sizeof(syn_t)];
    convert_helper_t helper;

    helper.pointer = data;
    helper.size = sizeof(data);

    std::memcpy(data, &syn, sizeof(syn_t));

    return helper;
}

convert_helper_t convert_to_data(file_info_list_t& list)
{
    data_buffer_t* data = new data_buffer_t[sizeof(file_info_list_t)];
    convert_helper_t helper;

    helper.pointer = data;
    helper.size = sizeof(data);

    std::memcpy(data, &list, sizeof(file_info_list_t));

    return helper;
}

convert_helper_t convert_to_data(file_info_list_t const& list)
{
    data_buffer_t* data = new data_buffer_t[sizeof(file_info_list_t)];
    convert_helper_t helper;

    helper.pointer = data;
    helper.size = sizeof(data);

    std::memcpy(data, &list, sizeof(file_info_list_t));

    return helper;
}