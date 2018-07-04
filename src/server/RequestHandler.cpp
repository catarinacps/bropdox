#include "server/RequestHandler.hpp"

RequestHandler::RequestHandler(
    sockaddr_in client_sock_address,
    port_t port_p,
    device_t dev,
    std::string const& address)
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
        auto file_data = this->sock_handler.wait_packet<bdu::file_data_t>();
        this->log("Received the requested file name");

        this->send_file(file_data->file.name);
    } break;
    case bdu::req::receive: {
        auto file_data = this->sock_handler.wait_packet<bdu::file_data_t>();
        this->log("Received the to-be-received file_data");

        if (!(file_data->num_packets > 0)) {
            bdu::ack_t ack(false);
            this->sock_handler.send_packet(&ack);
            this->log("Bad number of packets, sending false ack back...");

            return false;
        } else {
            bdu::ack_t ack(true);
            this->sock_handler.send_packet(&ack);
        }

        this->receive_file(file_data->file.name, file_data->num_packets);
    } break;
    case bdu::req::del: {
        auto file_data = this->sock_handler.wait_packet<bdu::file_data_t>();
        this->log("Received the to-be-deleted file name");

        this->delete_file(file_data->file.name);
    } break;
    case bdu::req::list: {
        this->log("Initiating list server process");

        auto ack = this->sock_handler.wait_packet<bdu::ack_t>();

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
    auto has_next_file = true;

    // Receive the modified files and overwrite them
    do {
        // Receives the file_data_t
        this->log("Receiving events...");
        auto file_event = this->sock_handler.wait_packet<bdu::file_event_t>();

        if (file_event->event == Event::close) {
            has_next_file = false;
        } else {
            switch (file_event->event) {
            case Event::create:
            case Event::modify: {
                bdu::ack_t ack(this->file_handler.check_freshness(file_event->file));
                this->sock_handler.send_packet(&ack);
                if (!ack.confirmation) {
                    this->log("I dont need old files lol");
                    continue;
                }

                auto file_data = this->sock_handler.wait_packet<bdu::file_data_t>();
                this->log("Received the to-be-received file_data");

                if (!(file_data->num_packets > 0)) {
                    bdu::ack_t ack_packt(false);
                    this->sock_handler.send_packet(&ack_packt);
                    this->log("Bad number of packets, sending false ack back...");
                } else {
                    bdu::ack_t ack_packt(true);
                    this->sock_handler.send_packet(&ack_packt);
                    this->log(file_data->file.name);
                    this->receive_file(file_data->file.name, file_data->num_packets);
                }
                break;
            }
            case Event::remove: {
                bdu::ack_t ack(true);
                this->sock_handler.send_packet(&ack);

                auto file_data = this->sock_handler.wait_packet<bdu::file_data_t>();
                this->log("Received the to-be-deleted file name");
                this->log(file_data->file.name);
                this->delete_file(file_data->file.name);
                break;
            }
            default:
                this->log("Unexpected event");
            }
        }

        // If the received file_data isn't empty, this field will be 'true'
    } while (has_next_file);
    // Iterates while there is an incoming file_data

    this->log("Yay!");

    auto all_files = this->file_handler.get_file_info_list();

    // Send to the client all files new to him
    for (auto& file_info : all_files) {
        this->log(file_info.file.name);
        this->sock_handler.send_packet(&file_info);

        auto ack = this->sock_handler.wait_packet<bdu::ack_t>();
        if (!ack) {
            this->log("Timeouted ack");
            continue;
        } else if (!ack->confirmation) {
            this->log("Client already has file");
            continue;
        }
        auto file_data = this->sock_handler.wait_packet<bdu::file_data_t>();
        this->log("consistency check");

        this->send_file(file_info.file.name);
        this->log("Sent file");
    }

    bdu::file_data_t last_file;
    this->sock_handler.send_packet(&last_file);
    this->log("Sent last file");

    auto ack = this->sock_handler.wait_packet<bdu::ack_t>();
    if (ack) {
        if (!ack->confirmation) {
            this->log("Syncing failure");
            return;
        }
    } else {
        this->log("No ack recieved");
    }

    this->log("Finished syncing");
    return;
}

void RequestHandler::send_file(char const* file)
{
    long int file_size_in_packets = 0;
    bdu::file_info finfo;

    std::vector<std::unique_ptr<bdu::packet_t>> packets;

    try {
        this->log(file);
        packets = this->file_handler.read_file(file, file_size_in_packets);
    } catch (bdu::file_does_not_exist const& e) {
        std::cerr << e.what() << '\n';

        bdu::file_data_t file_data(bdu::file_info(), 0);
        this->sock_handler.send_packet(&file_data);

        this->log("Requested file doesn't exist");
        return;
    } catch (std::ios::failure const& e) {
        std::cerr << e.what() << '\n';

        bdu::file_data_t file_data(bdu::file_info(), 0);
        this->sock_handler.send_packet(&file_data);

        this->log("IOS failure");
        return;
    }

    // If all goes well, the server sends the complete file_info to the client
    finfo = this->file_handler.get_file_info(file);
    bdu::file_data_t file_data(finfo, file_size_in_packets);
    this->sock_handler.send_packet(&file_data);
    this->log("Sent the requested file_info");

    // Packet sending loop
    for (auto const& packet : packets) {
        this->sock_handler.send_packet(packet.get());
        usleep(15);
    }
    this->log("Finished sending the packets");

    // The RequestHandler then procedes to wait for the Client's ack, which will contain the
    // number of packets that the client received.
    // This number of receveid packets will indicate a possible missing packet in the transmission,
    // calling for a repeat of the send_file() operation.
    auto ack = this->sock_handler.wait_packet<bdu::ack_t>();

    if (!ack->confirmation) {
        this->log("Failure sending the file, trying again...");
        this->send_file(file);
    } else {
        this->log("Success sending the file");
    }

    return;
}

void RequestHandler::receive_file(char const* file, unsigned int const packets_to_be_received)
{
    unsigned int received_packet_number = 0;

    // Uses said number of packets to declare an array of byte_t pointers
    // pointing to the received data
    std::vector<std::unique_ptr<bdu::packet_t>> recv_file(packets_to_be_received);

    // Packet receiving loop
    for (auto& packet : recv_file) {
        auto received_packet = this->sock_handler.wait_packet<bdu::packet_t>();
        // If the received packet is NULL, we do nothing
        if (received_packet) {
            // Can the packets arrive in another order?
            packet = std::move(received_packet);
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
        this->sock_handler.send_packet(&ack);
    } else {
        this->log("Failure receiving the file");

        bdu::ack_t ack(false);
        this->sock_handler.send_packet(&ack);
    }

    return;
}

void RequestHandler::delete_file(char const* file)
{
    if (!this->file_handler.delete_file(file)) {
        this->log("Failure deleting the file");

        bdu::ack_t ack(false);
        this->sock_handler.send_packet(&ack);
    } else {
        this->log("Success deleting the file");

        bdu::ack_t ack(true);
        this->sock_handler.send_packet(&ack);
    }

    return;
}

void RequestHandler::list_files()
{
    auto file_info_list = this->file_handler.get_file_info_list();

    bdu::file_data_t dummy_data(bdu::file_info(), file_info_list.size());
    this->sock_handler.send_packet(&dummy_data);
    this->log("Sent the number of files in the directory");

    // File_data sending loop
    for (auto& data : file_info_list) {
        this->sock_handler.send_packet(&data);
        usleep(15);
    }
    this->log("Finished sending the packets");

    auto ack = this->sock_handler.wait_packet<bdu::ack_t>();

    if (!ack->confirmation) {
        this->log("Failure sending the file data, trying again...");
        this->list_files();
    } else {
        this->log("Success sending the file data");
    }

    return;
}

void RequestHandler::log(char const* message)
{
    printf("RequestHandler [UID: %s]: %s\n", this->client_id.c_str(), message);
}
