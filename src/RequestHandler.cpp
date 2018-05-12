#include "../include/RequestHandler.hpp"

RequestHandler::RequestHandler(sockaddr_un client_address, std::string address)
{
    this->client_id = address;

    //! There are situations in which this constructor will fail to build
    //! a socket because the socket will already exist.
    sock_handler = new SocketHandler(client_address, address);
}

bool RequestHandler::wait_request(req req_type, struct file_info const& finfo)
{
    data_buffer_t* data;

    data = this->sock_handler->wait_packet(sizeof(handshake_t));
    if (data == NULL) {
        return false;
    }

    // TODO: init client info/folder if necessary

    switch (req_type) {
    case req::sync: {
        this->sync_server();
    } break;
    case req::send: {
        this->send_file(finfo.name);
    } break;
    case req::receive: {
        this->receive_file(finfo.name);
    } break;
    default:
        printf("Something went wrong...\n");
        return false;
    }

    return true;
}

void RequestHandler::sync_server()
{
    //stuff
}

void RequestHandler::send_file(char const* file)
{
    //TODO: FileHandler returns an array of packets relative to the file parameter
    // This behaviour will be represented as the following array
    packet_t packets[8]; // Arbitrary number
    //TODO: FileHandler returns the size in bytes of said file
    // This behaviour will be represented as the following unsigned int
    unsigned int file_size = 34534; // Arbitrary number

    convert_helper_t packet_to_be_sent;
    data_buffer_t* returned_packet;
    syn_t syn;
    ack_t* returned;

    syn.num_packets = 8; // Arbitrary number
    syn.file_size = file_size;
    packet_to_be_sent = convert_to_data(syn);
    this->sock_handler->send_packet(packet_to_be_sent.pointer, packet_to_be_sent.size);

    // Packet sending loop
    for (packet_t const& packet : packets) {
        packet_to_be_sent = convert_to_data(packet);
        // MAYBE THE FOLLOWING WILL GO TERRIBLY BAD because the sizeof part
        this->sock_handler->send_packet(packet_to_be_sent.pointer, packet_to_be_sent.size);
    }

    // The RequestHandler then procedes to wait for the Client's ack, which will contain the
    // number of packets that the client received.
    // This number of receveid packets will indicate a possible missing packet in the transmission,
    // calling for a repeat of the send_file() operation.
    returned_packet = this->sock_handler->wait_packet(sizeof(ack_t));
    returned = convert_to_ack(returned_packet);

    if (returned->num_packets != syn.num_packets) {
        this->send_file(file);
    }

    delete returned;
    delete returned_packet;

    return;
}

void RequestHandler::receive_file(char const* file)
{
    ack_t* syn;
    ack_t ack;
    packet_t* received;
    convert_helper_t helper;
    data_buffer_t *syn_packet, *received_packet;
    unsigned int packets_to_be_received, received_packet_number = 0;

    // Waits for the client's syn packet to determine the expected number of packets
    syn_packet = this->sock_handler->wait_packet(sizeof(ack_t));
    syn = convert_to_ack(syn_packet);
    packets_to_be_received = syn->num_packets;

    // Uses said number of packets to declare an array of data_buffer_t pointers
    // pointing to the received data
    data_buffer_t* recv_file[packets_to_be_received];

    // Packet receiving loop
    for (unsigned int i = 0; i < packets_to_be_received; i++) {
        received_packet = this->sock_handler->wait_packet(sizeof(packet_t));

        // If the received packet is NULL, we do nothing
        if (received_packet != NULL) {
            received = convert_to_packet(received_packet);
            recv_file[received->num] = received->data;
            received_packet_number++;
        }

        delete received_packet;
    }
    delete received;

    // After receiving all packets, we send an ack with the number of packets we received.
    // If the number doesnt match the expected number, the client should do something about it.
    ack.num_packets = received_packet_number;
    helper = convert_to_data(ack);
    this->sock_handler->send_packet(helper.pointer, helper.size);

    // And we do nothing if the number doesnt match
    if (received_packet_number == packets_to_be_received) {
        //TODO: Send to FileHandler the received recv_file array
    }

    for (auto const& point : recv_file) delete point;
    delete syn;
    delete syn_packet;
    delete[] recv_file;

    return;
}