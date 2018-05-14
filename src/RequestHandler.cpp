#include "../include/RequestHandler.hpp"

RequestHandler::RequestHandler(sockaddr_un client_address, std::string address)
{
    this->client_id = address;

    //! There are situations in which this constructor will fail to build
    //! a socket because the socket will already exist.
    sock_handler = new SocketHandler(client_address, address);

    file_handler = new FileHandler(address);
}

bool RequestHandler::wait_request(req req_type, struct file_info const& finfo)
{
    data_buffer_t* data;

    data = this->sock_handler->wait_packet(sizeof(handshake_t));
    if (data == NULL) {
        return false;
    }

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
    int i = 0, dummy_file_info_num;
    bool has_next_file_list = false;
    double num_file_lists;
    data_buffer_t* received_data;
    convert_helper_t helper;
    file_info_list_t* received_list;
    std::vector<file_info> modified_files, diff_files, to_be_sent_files;
    std::string name;
    std::vector<file_info>::iterator it1, it2;


    /*************************************************************************/
    // Receive the modified files and overwrite them
    do {
        // Receives the file_info_list
        received_data = this->sock_handler->wait_packet(sizeof(file_info_list_t));
        received_list = convert_to_file_list(received_data);
        delete received_data;

        // If the received file_info_list has another incoming list, this field will be 'true'
        has_next_file_list = received_list->has_next;

        // Pushes every received file_info to the modified_files array
        while (i < MAX_FILE_LIST_SIZE && received_list->file_list[i].size > 0) {
            modified_files.push_back(received_list->file_list[i]);
            i++;
        }
    } while (has_next_file_list);
    // Iterates while there is an incoming list

    // Then we proceed to receive every file that the client has sent us
    for (auto const& file : modified_files) {
        this->receive_file(file.name);
    }

    /*************************************************************************/
    // Send the complete file list minus the just received modified files
    
    std::vector<file_info> server_files = this->file_handler->get_file_info_list();

    // Sorts the modified_files and server_files vectors so that the set_difference method works.
    std::sort(server_files.begin(), server_files.end());
    std::sort(modified_files.begin(), modified_files.end());

    // Calculates the difference between said vector and store it in diff_files.
    std::set_difference(
        server_files.begin(), server_files.end(),
        modified_files.begin(), modified_files.end(),
        back_inserter(diff_files));
    
    // Calculates the ceiling between the mod division of diff_files with 10.
    num_file_lists = ceil(diff_files.size() % MAX_FILE_LIST_SIZE);
    // Calculates the number of file_info fields that we'll have to fill as part of 'padding' the 
    // last file_info_list.
    dummy_file_info_num = num_file_lists * MAX_FILE_LIST_SIZE - diff_files.size();
    // Fills out the 'padding' part
    if (dummy_file_info_num > 0) {
        file_info dummy;
        for (i = 0; i < dummy_file_info_num; i++) diff_files.push_back(dummy);
    }

    // Declares the file_info_list array using the previously ceil'd value
    file_info_list_t file_list_array[(int)num_file_lists];

    // Initializes two diff_files iterators so we can copy its contents to file_list_array
    it1 = diff_files.begin();
    it2 = diff_files.begin();
    // And we advance the second iterator so there's 10 elements between the two iteratos
    std::advance(it2, MAX_FILE_LIST_SIZE - 1);
    // For each item in the array...
    for (i = 0; i < num_file_lists; i++) {
        // Copies 10 elements to the current item array
        std::copy(it1, it2, file_list_array[i].file_list);
        // Sets the has_next flag to true
        file_list_array[i].has_next = true;
        // And advances both iterators to the next 10 elements
        std::advance(it1, MAX_FILE_LIST_SIZE - 1);
        std::advance(it2, MAX_FILE_LIST_SIZE - 1);
    }
    // Sets the last has_next as false
    file_list_array[((int)num_file_lists) - 1].has_next = false;

    // Then iterates over the array and sends every item to the client
    for (auto const& list : file_list_array) {
        helper = convert_to_data(list);
        this->sock_handler->send_packet(helper.pointer, helper.size);
        delete helper.pointer;
        usleep(15);
    }

    /*************************************************************************/
    // Receive the actual file list that the client does not yet have
    // It's basically the same as above

    do {
        received_data = this->sock_handler->wait_packet(sizeof(file_info_list_t));
        received_list = convert_to_file_list(received_data);
        delete received_data;

        has_next_file_list = received_list->has_next;

        while (i < MAX_FILE_LIST_SIZE && received_list->file_list[i].size > 0) {
            to_be_sent_files.push_back(received_list->file_list[i]);
            i++;
        }
    } while (has_next_file_list);

    /*************************************************************************/
    // Send to the client all files new to him

    for (auto const& file : to_be_sent_files) {
        this->send_file(file.name);
    }

    return;
}

void RequestHandler::send_file(char const* file)
{
    long int file_size_in_packets;
    packet_t** packets = this->file_handler->get_file(file, file_size_in_packets);

    convert_helper_t packet_to_be_sent;
    data_buffer_t* returned_packet;
    syn_t syn;
    ack_t* returned;

    syn.num_packets = file_size_in_packets;
    syn.file_size = file_size_in_packets * PACKETSIZE;
    packet_to_be_sent = convert_to_data(syn);
    this->sock_handler->send_packet(packet_to_be_sent.pointer, packet_to_be_sent.size);
    delete packet_to_be_sent.pointer;

    // Packet sending loop
    for (int i = 0; i < file_size_in_packets; i++) {
        packet_to_be_sent = convert_to_data(*packets[i]);
        //! MAYBE THE FOLLOWING WILL GO TERRIBLY BAD because the sizeof part
        this->sock_handler->send_packet(packet_to_be_sent.pointer, packet_to_be_sent.size);
        delete packet_to_be_sent.pointer;
        usleep(15);
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
    packet_t* received = NULL;
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
            delete received;
        }

        delete received_packet;
    }

    // After receiving all packets, we send an ack with the number of packets we received.
    // If the number doesnt match the expected number, the client should do something about it.
    ack.num_packets = received_packet_number;
    helper = convert_to_data(ack);
    this->sock_handler->send_packet(helper.pointer, helper.size);

    // And we do nothing if the number doesnt match
    if (received_packet_number == packets_to_be_received) {
        this->file_handler->write_file(file, recv_file, syn->num_packets);
    }

    for (auto const& point : recv_file)
        delete point;
    delete syn;
    delete helper.pointer;
    delete syn_packet;
    delete[] recv_file;

    return;
}