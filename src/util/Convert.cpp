#include "util/Convert.hpp"
#include <iostream>
namespace bdu {

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

std::unique_ptr<file_data_t> convert_to_file_data(byte_t* data)
{
    std::cout << "A" << std::endl;
    auto file = std::make_unique<file_data_t>();
    std::cout << "B" << std::endl;
    std::memcpy(file.get(), data, sizeof(file_data_t));
    std::cout << "C" << std::endl;
    return file;
}
}