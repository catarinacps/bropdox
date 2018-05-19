#include "../include/RequestHandler.hpp"

RequestHandler::RequestHandler(sockaddr_in client_address, in_port_t port, std::string address)
{
    this->client_id = address;

    //! There are situations in which this constructor will fail to build
    //! a socket because the socket will already exist.
    sock_handler = new SocketHandler(port, client_address);

    file_handler = new FileHandler(address, 0);
}

bool RequestHandler::handle_request(req req_type)
{
    switch (req_type) {
    case req::sync: {
        this->sync_server();
    } break;
    case req::send: {
        data_buffer_t* data = this->sock_handler->wait_packet(sizeof(file_data_t));
        file_data_t* finfo = convert_to_file_data(data);


        delete[] data;
        this->send_file(finfo->file.name);
    } break;
    case req::receive: {
        data_buffer_t* data = this->sock_handler->wait_packet(sizeof(file_data_t));
        file_data_t* finfo = convert_to_file_data(data);

        if (!(finfo->num_packets > 0)) {
            ack_t ack(false);
            this->sock_handler->send_packet(&ack, sizeof(ack_t));

            return false;
        } else {
            ack_t ack(true);
            this->sock_handler->send_packet(&ack, sizeof(ack_t));
        }

        delete[] data;
        this->receive_file(finfo->file.name, finfo->num_packets);
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

    // !
    //TODO: Fix this whole method
    // !

    /*************************************************************************/
    // Receive the modified files and overwrite them
    do {
        // Receives the file_info_list
        received_data = this->sock_handler->wait_packet(sizeof(file_info_list_t));
        received_list = convert_to_file_list(received_data);
        delete[] received_data;

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
        //this->receive_file(file.name);
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
        delete[] helper.pointer;
        usleep(15);
    }

    /*************************************************************************/
    // Receive the actual file list that the client does not yet have
    // It's basically the same as above

    do {
        received_data = this->sock_handler->wait_packet(sizeof(file_info_list_t));
        received_list = convert_to_file_list(received_data);
        delete[] received_data;

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
    data_buffer_t* returned_ack;
    ack_t* ack;

    long int file_size_in_packets;
    packet_t** packets = this->file_handler->get_file(file, file_size_in_packets);

    struct file_info finfo(file);
    file_data_t file_data(finfo, file_size_in_packets);
    this->sock_handler->send_packet(&file_data, sizeof(file_data_t));

    // Packet sending loop
    for (int i = 0; i < file_size_in_packets; i++) {
        //! MAYBE THE FOLLOWING WILL GO TERRIBLY BAD because the sizeof part
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

void RequestHandler::receive_file(char const* file, unsigned int packets_to_be_received)
{
    packet_t* received = NULL;
    data_buffer_t* received_packet;
    unsigned int received_packet_number = 0;

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

        delete[] received_packet;
    }

    // After receiving all packets, we send an ack with true if we received all the packets or
    // false if we didn't.
    // If the number doesnt match the expected number, the client should do something about it.
    // Also, we do nothing if the number doesnt match.
    if (received_packet_number == packets_to_be_received) {
        printf("RequestHandler: Success receiving the file");

        ack_t ack(true);
        this->sock_handler->send_packet(&ack, sizeof(ack_t));

        this->file_handler->write_file(file, recv_file, packets_to_be_received);
    } else {
        printf("RequestHandler: Failure receiving the file");

        ack_t ack(false);
        this->sock_handler->send_packet(&ack, sizeof(ack_t));
    }

    for (auto const& point : recv_file)
        delete[] point;
    delete[] recv_file;

    return;
}