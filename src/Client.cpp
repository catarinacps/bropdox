#include "../include/Client.hpp"

Client::Client(char* uid, char* host, int port)
{
    sprintf(this->userid, uid);
    if (login_server(host, port))
        this->logged_in = 1;

    this->file_handler = new FileHandler(this->userid);
}

int Client::login_server(char* host, int port)
{
    // The behaviour supposed to take place in this method is already covered when the 
    // client sends a handshake to the server

    return 1;
}

void Client::sync_client()
{
}

void Client::send_file(char const* file)
{
    data_buffer_t* returned_ack;
    ack_t* ack;

    long int file_size_in_packets;
    packet_t** packets = this->file_handler->get_file(file, file_size_in_packets);

    struct file_info finfo = this->file_handler->get_file_info(file);
    file_data_t file_data(finfo, file_size_in_packets);
    this->sock_handler->send_packet(&file_data, sizeof(file_data_t));

    // Packet sending loop
    for (int i = 0; i < file_size_in_packets; i++) {
        this->sock_handler->send_packet(packets[i], sizeof(packet_t));
        usleep(15);
    }

    // The RequestHandler then procedes to wait for the Client's ack, which will contain the
    // number of packets that the client received.
    // This number of receveid packets will indicate a possible missing packet in the transmission,
    // calling for a repeat of the send_file() operation.
    returned_ack = this->sock_handler->wait_packet(sizeof(ack_t));
    ack = convert_to_ack(returned_ack);

    if (!ack->confirmation) {
        this->send_file(file);
    }

    delete ack;
    delete[] returned_ack;

    return;
}

void Client::get_file(char const* file)
{
    packet_t* received = NULL;
    data_buffer_t* received_packet;
    unsigned int received_packet_number = 0;
    file_data_t* received_finfo;

    struct file_info finfo = this->file_handler->get_file_info(file);
    file_data_t file_data(finfo, 0);
    this->sock_handler->send_packet(&file_data, sizeof(file_data_t));

    received_packet = this->sock_handler->wait_packet(sizeof(file_data_t));
    received_finfo = convert_to_file_data(received_packet);
    delete received_packet;

    unsigned int packets_to_be_received = received_finfo->num_packets;

    // Uses said number of packets to declare an array of data_buffer_t pointers
    // pointing to the received data
    data_buffer_t* recv_file[packets_to_be_received];

    // Packet receiving loop
    for (unsigned int i = 0; i < packets_to_be_received; i++) {
        received_packet = this->sock_handler->wait_packet(sizeof(packet_t));

        // If the received packet is NULL, we do nothing
        if (received_packet != nullptr) {
            received = convert_to_packet(received_packet);
            //TODO: Copy the received data array to the recv_file array
            recv_file[received->num] = received->data;
            received_packet_number++;
        }

        delete[] received_packet;
    }

    // After receiving all packets, we send an ack with true if we received all the packets or
    // false if we didn't.
    // If the number doesnt match the expected number, the client should do something about it.
    // Also, we do nothing if the number doesnt match.
    if (received_packet_number == packets_to_be_received) {
        printf("Success receiving the file\n");

        ack_t ack(true);
        this->sock_handler->send_packet(&ack, sizeof(ack_t));

        this->file_handler->write_file(file, recv_file, packets_to_be_received);
    } else {
        printf("Failure receiving the file");

        ack_t ack(false);
        this->sock_handler->send_packet(&ack, sizeof(ack_t));
    }
    printf("sup\n");

    /* for (auto const& point : recv_file)
        delete[] point; */

    return;
}

void Client::delete_file(char const* file)
{
}

void Client::close_session()
{
}
