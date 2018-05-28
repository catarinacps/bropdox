#include "../include/bropdoxUtil.hpp"

int init_unix_socket(struct sockaddr_in& sock, in_port_t port) throw()
{
    int socket_id;
    if ((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Error while initializing the socket\n");
        throw socket_bad_create();
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = INADDR_ANY;
    bzero(&(sock.sin_zero), 8);

    return socket_id;
}

int init_unix_socket(struct sockaddr_in& sock, in_port_t port, hostent& server) throw()
{
    int socket_id;
    if ((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Error while initializing the socket\n");
        throw socket_bad_create();
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr = *((struct in_addr*)server.h_addr);
    bzero(&(sock.sin_zero), 8);

    return socket_id;
}

std::unique_ptr<handshake_t> convert_to_handshake(byte_t* data)
{
    auto hand = std::make_unique<handshake_t>();
    std::memcpy(hand.get(), data, sizeof(handshake_t));
    return hand;
}

std::unique_ptr<ack_t> convert_to_ack(byte_t* data)
{
    auto ack = std::make_unique<ack_t>();
    std::memcpy(ack.get(), data, sizeof(ack_t));
    return ack;
}

std::unique_ptr<syn_t> convert_to_syn(byte_t* data)
{
    auto syn = std::make_unique<syn_t>();
    std::memcpy(syn.get(), data, sizeof(syn_t));
    return syn;
}

std::unique_ptr<packet_t> convert_to_packet(byte_t* data)
{
    auto packet = std::make_unique<packet_t>();
    std::memcpy(packet.get(), data, sizeof(packet_t));
    return packet;
}

std::unique_ptr<file_info_list_t> convert_to_file_list(byte_t* data)
{
    auto list = std::make_unique<file_info_list_t>();
    std::memcpy(list.get(), data, sizeof(file_info_list_t));
    return list;
}

std::unique_ptr<file_data_t> convert_to_file_data(byte_t* data)
{
    auto file = std::make_unique<file_data_t>();
    std::memcpy(file.get(), data, sizeof(file_data_t));
    return file;
}
