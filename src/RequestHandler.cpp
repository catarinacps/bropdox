#include "../include/RequestHandler.hpp"

RequestHandler::RequestHandler(sockaddr_in client_sock_address, port_t port_p, unsigned short int dev, std::string const& address)
    : sock_handler(port_p, client_sock_address)
    , file_handler(address, 0)
    , client_id(address)
    , device(dev)
    , port(port_p)
{
}

unsigned short int RequestHandler::get_device()
{
    return this->device;
}

bool RequestHandler::handle_request(req req_type)
{
    this->sock_handler.flush_address_queue();
    
    switch (req_type) {
    case req::sync: {
        this->sync_server();
    } break;
    case req::send: {
        auto data = this->sock_handler.wait_packet(sizeof(file_data_t));
        auto finfo = convert_to_file_data(data.get());
        this->log("Received the requested file name");

        this->send_file(finfo->file.name);
    } break;
    case req::receive: {
        auto data = this->sock_handler.wait_packet(sizeof(file_data_t));
        auto finfo = convert_to_file_data(data.get());
        this->log("Received the to-be-received file_info");

        if (!(finfo->num_packets > 0)) {
            ack_t ack(false);
            this->sock_handler.send_packet(&ack, sizeof(ack_t));
            this->log("Bad number of packets, sending false ack back...");

            return false;
        } else {
            ack_t ack(true);
            this->sock_handler.send_packet(&ack, sizeof(ack_t));
        }

        this->receive_file(finfo->file.name, finfo->num_packets);
    } break;
    case req::del: {
        auto data = this->sock_handler.wait_packet(sizeof(file_data_t));
        auto finfo = convert_to_file_data(data.get());
        this->log("Received the to-be-deleted file name");

        this->delete_file(finfo->file.name);
    } break;
    default:
        this->log("Something went wrong...");
        return false;
    }

    return true;
}

void RequestHandler::sync_server()
{
    /* int i = 0, dummy_file_info_num;
    bool has_next_file_list = false;
    double num_file_lists;
    byte_t* received_data;
    convert_helper_t helper;
    file_info_list_t* received_list;
    std::vector<file_data_t> modified_files, diff_files, to_be_sent_files;
    std::string name;
    std::vector<file_data_t>::iterator it1, it2;

    // !
    //FIXME: Fix this whole method
    // !

    /************************************************************************
    // Receive the modified files and overwrite them
    do {
        // Receives the file_info_list
        received_data = this->sock_handler.wait_packet(sizeof(file_info_list_t));
        received_list = convert_to_file_list(received_data);
        delete[] received_data;

        // Pushes every received file_info to the modified_files array
        while (i < MAX_FILE_LIST_SIZE && received_list->file_list[i].num_packets > 0) {
            modified_files.push_back(received_list->file_list[i]);
            i++;
        }

        // If the received file_info_list has another incoming list, this field will be 'true'
        has_next_file_list = received_list->has_next;
        delete received_list;
    } while (has_next_file_list);
    // Iterates while there is an incoming list

    // Then we proceed to receive every file that the client has sent us
    for (auto const& file_d : modified_files) {
        this->receive_file(file_d.file.name, file_d.num_packets);
    }

    /************************************************************************
    // Send the complete file list minus the just received modified files
    
    std::vector<file_info> server_files = this->file_handler.get_file_info_list();

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
        file_data_t dummy;
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
        this->sock_handler.send_packet(helper.pointer, helper.size);
        delete[] helper.pointer;
        usleep(15);
    }

    /************************************************************************
    // Receive the actual file list that the client does not yet have
    // It's basically the same as above

    do {
        received_data = this->sock_handler.wait_packet(sizeof(file_info_list_t));
        received_list = convert_to_file_list(received_data);
        delete[] received_data;

        has_next_file_list = received_list->has_next;

        while (i < MAX_FILE_LIST_SIZE && received_list->file_list[i].num_packets > 0) {
            to_be_sent_files.push_back(received_list->file_list[i]);
            i++;
        }
    } while (has_next_file_list);

    /************************************************************************
    // Send to the client all files new to him

    for (auto const& file_d : to_be_sent_files) {
        this->send_file(file_d.file.name);
    }

    return; */
}

void RequestHandler::send_file(char const* file)
{
    long int file_size_in_packets;
    struct file_info finfo;

    // If get_file returns nullptr, something has gone wrong
    packet_t** packets = this->file_handler.get_file(file, file_size_in_packets);
    if (packets == nullptr) {
        file_data_t file_data(finfo, file_size_in_packets);
        this->sock_handler.send_packet(&file_data, sizeof(file_data_t));
        this->log("Requested file doesn't exist");
        return;
    }
    // If all goes well, the server sends the complete file_info to the client
    finfo = this->file_handler.get_file_info(file);
    file_data_t file_data(finfo, file_size_in_packets);
    this->sock_handler.send_packet(&file_data, sizeof(file_data_t));
    this->log("Sent the requested file_info");

    // Packet sending loop
    for (int i = 0; i < file_size_in_packets; i++) {
        this->sock_handler.send_packet(packets[i], sizeof(packet_t));
        usleep(15);
    }
    this->log("Finished sending the packets");

    // The RequestHandler then procedes to wait for the Client's ack, which will contain the
    // number of packets that the client received.
    // This number of receveid packets will indicate a possible missing packet in the transmission,
    // calling for a repeat of the send_file() operation.
    auto returned_ack = this->sock_handler.wait_packet(sizeof(ack_t));
    auto ack = convert_to_ack(returned_ack.get());

    if (!ack->confirmation) {
        this->log("Failure sending the file, trying again...");
        this->send_file(file);
    } else {
        this->log("Success sending the file");
    }

    return;
}

//TODO: Check if packets can arrive in another order
void RequestHandler::receive_file(char const* file, unsigned int const packets_to_be_received)
{
    unsigned int received_packet_number = 0;

    // Uses said number of packets to declare an array of byte_t pointers
    // pointing to the received data
    std::vector<std::unique_ptr<packet_t>> recv_file(packets_to_be_received);

    // Packet receiving loop
    for (auto& packet : recv_file) {
        auto received_packet = this->sock_handler.wait_packet(sizeof(packet_t));

        // If the received packet is NULL, we do nothing
        if (received_packet != nullptr) {
            auto received = convert_to_packet(received_packet.get());

            // Can the packets arrive in another order?
            packet = std::move(received);
            received_packet_number++;
        }
    }

    // After receiving all packets, we send an ack with true if we received all the packets or
    // false if we didn't.
    // If the number doesnt match the expected number, the client should do something about it.
    // Also, we do nothing if the number doesnt match.
    if (received_packet_number == packets_to_be_received) {
        this->log("Success receiving the file");

        ack_t ack(true);
        this->sock_handler.send_packet(&ack, sizeof(ack_t));

        this->file_handler.write_file(file, recv_file);
    } else {
        this->log("Failure receiving the file");

        ack_t ack(false);
        this->sock_handler.send_packet(&ack, sizeof(ack_t));
    }

    return;
}

void RequestHandler::delete_file(char const* file)
{
    if (!this->file_handler.delete_file(file)) {
        this->log("Failure deleting the file");

        ack_t ack(false);
        this->sock_handler.send_packet(&ack, sizeof(ack_t));
    } else {
        this->log("Success deleting the file");

        ack_t ack(true);
        this->sock_handler.send_packet(&ack, sizeof(ack_t));
    }

    return;
}

void RequestHandler::log(char const* message)
{
    printf("RequestHandler [UID: %s]: %s\n", this->client_id.c_str(), message);
}
