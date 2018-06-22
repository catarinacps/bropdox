#include "server/RequestHandler.hpp"

RequestHandler::RequestHandler(sockaddr_in client_sock_address, port_t port_p, device_t dev, std::string const& address)
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

bool RequestHandler::handle_request(bdu::req req_type)
{
    switch (req_type) {
    case bdu::req::sync: {
        this->sync_server();
    } break;
    case bdu::req::send: {
        auto data = this->sock_handler.wait_packet(sizeof(bdu::file_data_t));
        if(data == NULL){
            std::cout << "Empty inside and outside"  << std::endl;
        }
        auto finfo = bdu::convert_to_file_data(data.get());
        this->log("Received the requested file name");

        this->send_file(finfo->file.name);
    } break;
    case bdu::req::receive: {
        auto data = this->sock_handler.wait_packet(sizeof(bdu::file_data_t));
        auto finfo = bdu::convert_to_file_data(data.get());
        this->log("Received the to-be-received file_info");

        if (!(finfo->num_packets > 0)) {
            bdu::ack_t ack(false);
            this->sock_handler.send_packet(&ack, sizeof(bdu::ack_t));
            this->log("Bad number of packets, sending false ack back...");

            return false;
        } else {
            bdu::ack_t ack(true);
            this->sock_handler.send_packet(&ack, sizeof(bdu::ack_t));
        }

        this->receive_file(finfo->file.name, finfo->num_packets);
    } break;
    case bdu::req::del: {
        auto data = this->sock_handler.wait_packet(sizeof(bdu::file_data_t));
        auto finfo = bdu::convert_to_file_data(data.get());
        this->log("Received the to-be-deleted file name");

        this->delete_file(finfo->file.name);
    } break;
    case bdu::req::list: {
         auto data = this->sock_handler.wait_packet(sizeof(bdu::file_data_t));
        auto finfo = bdu::convert_to_file_data(data.get());
        this->list_files();
    } break;
    default:
        this->log("Something went wrong...");
        return false;
    }

    return true;
}

void RequestHandler::sync_server()
{
    auto has_next_file = false;

    auto to_be_sent_files = this->file_handler.get_file_info_list();
    to_be_sent_files.emplace_back();
    
    // Receive the modified files and overwrite them
    do {
        // Receives the file_data_t
        auto file_data_bytes = this->sock_handler.wait_packet(sizeof(bdu::file_data_t));
        auto file_data = bdu::convert_to_file_data(file_data_bytes.get());

        if (file_data->num_packets == 0) {
            has_next_file = false;
        } else {
            bdu::ack_t ack(this->file_handler.check_freshness(file_data->file));

            auto it = std::find(to_be_sent_files.begin(), to_be_sent_files.end(), *file_data);
            if (it != to_be_sent_files.end()) {
                to_be_sent_files.erase(it);
            }

            this->sock_handler.send_packet(&ack, sizeof(bdu::ack_t));

            this->receive_file(file_data->file.name, file_data->num_packets);
            has_next_file = true;
        }

        // If the received file_data isn't empty, this field will be 'true'
    } while (has_next_file);
    // Iterates while there is an incoming file_data

    // Send to the client all files new to him
    for (auto& file_info : to_be_sent_files) {
        this->sock_handler.send_packet(&file_info, sizeof(bdu::file_data_t));

        auto ack_bytes = this->sock_handler.wait_packet(sizeof(bdu::ack_t));
        auto ack = bdu::convert_to_ack(ack_bytes.get());

        if (ack->confirmation) {
            this->send_file(file_info.file.name);
        }
    }

    return;
}

void RequestHandler::send_file(char const* file)
{
    long int file_size_in_packets;
    bdu::file_info finfo;


    std::vector<std::unique_ptr<bdu::packet_t>> packets;

    try {
        packets = this->file_handler.read_file(file, file_size_in_packets);
    } catch (bdu::file_does_not_exist const& e) {
        std::cerr << e.what() << '\n';

        bdu::file_data_t file_data(finfo, file_size_in_packets);
        this->sock_handler.send_packet(&file_data, sizeof(bdu::file_data_t));

        this->log("Requested file doesn't exist");
        return;
    } catch (std::ios::failure const& e) {
        std::cerr << e.what() << '\n';

        bdu::file_data_t file_data(finfo, file_size_in_packets);
        this->sock_handler.send_packet(&file_data, sizeof(bdu::file_data_t));
        
        this->log("IOS failure");
        return;
    }

    // If all goes well, the server sends the complete file_info to the client
    finfo = this->file_handler.get_file_info(file);
    bdu::file_data_t file_data(finfo, file_size_in_packets);
    this->sock_handler.send_packet(&file_data, sizeof(bdu::file_data_t));
    this->log("Sent the requested file_info");

    // Packet sending loop
    for (auto const& packet : packets) {
        this->sock_handler.send_packet(packet.get(), sizeof(bdu::packet_t));
        usleep(15);
    }
    this->log("Finished sending the packets");

    // The RequestHandler then procedes to wait for the Client's ack, which will contain the
    // number of packets that the client received.
    // This number of receveid packets will indicate a possible missing packet in the transmission,
    // calling for a repeat of the send_file() operation.
    auto returned_ack = this->sock_handler.wait_packet(sizeof(bdu::ack_t));
    auto ack = bdu::convert_to_ack(returned_ack.get());

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
    std::vector<std::unique_ptr<bdu::packet_t>> recv_file(packets_to_be_received);

    // Packet receiving loop
    for (auto& packet : recv_file) {
        auto received_packet = this->sock_handler.wait_packet(sizeof(bdu::packet_t));

        // If the received packet is NULL, we do nothing
        if (received_packet != nullptr) {
            auto received = bdu::convert_to_packet(received_packet.get());

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

        auto write = this->file_handler.write_file(file, std::move(recv_file));

        bdu::ack_t ack(write);
        this->sock_handler.send_packet(&ack, sizeof(bdu::ack_t));
    } else {
        this->log("Failure receiving the file");

        bdu::ack_t ack(false);
        this->sock_handler.send_packet(&ack, sizeof(bdu::ack_t));
    }

    return;
}

void RequestHandler::delete_file(char const* file)
{
    if (!this->file_handler.delete_file(file)) {
        this->log("Failure deleting the file");

        bdu::ack_t ack(false);
        this->sock_handler.send_packet(&ack, sizeof(bdu::ack_t));
    } else {
        this->log("Success deleting the file");

        bdu::ack_t ack(true);
        this->sock_handler.send_packet(&ack, sizeof(bdu::ack_t));
    }

    return;
}

void RequestHandler::log(char const* message)
{
    printf("RequestHandler [UID: %s]: %s\n", this->client_id.c_str(), message);
}

void RequestHandler::list_files(){ 
    std::cout << "~List of files~" << std::endl;   
    
    auto file_info_list = this->file_handler.get_file_info_list();
    
    for (auto& file_info :file_info_list) {
        std::cout << file_info.file.name << std::endl;
    }

    int number_of_files = file_info_list.size();
    std::cout << number_of_files << std::endl;
    this->sock_handler.send_packet(&number_of_files, sizeof(long int)); //???
    this->log("Sent the number of files in the directory");

}