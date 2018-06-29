#include "client/Client.hpp"

Client::Client(char* uid)
    : userid(uid)
    , file_handler(userid)
    , watcher(userid)
{
}

void Client::command_line_interface()
{
    std::string input;
    unsigned int length;
    std::vector<std::string> tokens;

    while (true) {
        std::cout << "$ ";
        std::getline(std::cin, input);

        length = input.length();
        if (length > 0) {
            boost::split(tokens, input, boost::is_any_of(" "));

            this->log("Parsing input");
            this->parse_input(tokens);
        }
    }
}

bool Client::parse_input(std::vector<std::string> tokens)
{
    std::string command(tokens[0]);

    //TODO: list_server and list_client

    if (command == "connect") {
        std::string address(tokens[1]);
        std::string port_s(tokens[2]);
        port_t port = atoi(port_s.c_str());

        return this->connect_to_server(address.c_str(), port);
    } else if (command == "upload") {
        std::string file_path(tokens[1]);

        // We start by sending a handshake containing the request to the server
        if (!this->send_handshake(bdu::req::receive)) {
            return false;
        }

        return this->send_file(file_path.c_str());
    } else if (command == "download") {
        std::string file_path(tokens[1]);

        // We start by sending a handshake containing the request to the server
        if (!this->send_handshake(bdu::req::send)) {
            return false;
        }

        return this->get_file(file_path.c_str());
    } else if (command == "delete") {
        std::string file_path(tokens[1]);
        // We start by sending a handshake containing the request to the server
        if (!this->send_handshake(bdu::req::del)) {
            return false;
        }

        return this->delete_file(file_path.c_str());
    } else if (command == "exit") {
        return this->close_session();
    } else if (command == "login") {
        if (this->send_handshake(bdu::req::login)) {
            std::thread daemon([&]() {
                this->watcher.run();
                
                while (this->watcher.is_running()) {
                    // Do a sync using the modified files queue?
                    this->log("Im awake!");
                    auto modified_files = this->watcher.get_events();

                    std::cout << modified_files.front().file.name << " is the file!";

                    std::this_thread::sleep_for(std::chrono::seconds(DAEMON_SLEEP_SECONDS));
                } 
            });
            daemon.detach();
            return true;
        } else {
            return false;
        }
    } else if (command == "ls") {
        if (!this->send_handshake(bdu::req::list)) {
            return false;
        }

        return this->list_server_files();
    } else {
        std::cout << "usage:\n"
                  << "login <hostname> <port>\n"
                  << "upload <file path>\n"
                  << "download <file>\n"
                  << "ls\n"
                  << "exit\n";
        return false;
    }

    this->log("Unknown command");
    return false;
}

bool Client::connect_to_server(char const* host, int port)
{
    this->server = gethostbyname(host);
    if (server == NULL) {
        this->log("Server not found");
        return false;
    }

    try {
        this->sock_handler_server = std::move(SocketHandler(port, this->server));
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        this->log("Failed to connect with the server");
        return false;
    }

    this->log("Connected to server");

    //TODO: Sync the client with all the server's files
    //TODO: Start sync daemon

    return true;
}

bool Client::sync_client()
{
    return false;
}

bool Client::send_file(char const* file)
{
    long int file_size_in_packets = 0;

    // Get the file data and file size in packets
    auto packets = this->file_handler.read_file(file, file_size_in_packets);

    // We then send the file data to the server
    bdu::file_data_t file_data(this->file_handler.get_file_info(file), file_size_in_packets);
    this->sock_handler_req->send_packet(&file_data);

    // And wait for the server's ACK
    auto returned_ack = this->sock_handler_req->wait_packet(sizeof(bdu::ack_t));
    auto ack = bdu::convert_from_bytes<bdu::ack_t>(returned_ack.get());

    // If it's 'false' we abort
    if (!ack->confirmation) {
        this->log("Bad ACK received");
        return false;
    }

    // Packet sending loop
    for (auto const& packet : packets) {
        this->sock_handler_req->send_packet(packet.get());
        usleep(15);
    }
    this->log("Finished sending the file");

    // The Client then procedes to wait for the RequestHandler's ack, which will contain the
    // number of packets that he received.
    // This number of received packets will indicate a possible missing packet in the transmission,
    // calling for a repeat of the send_file() operation.

    returned_ack = this->sock_handler_req->wait_packet(sizeof(bdu::ack_t));
    ack = bdu::convert_from_bytes<bdu::ack_t>(returned_ack.get());

    // If it's 'false' we try again
    if (!ack->confirmation) {
        this->log("Bad ACK received, sending file again...");
        this->send_file(file);
    } else {
        this->log("Success uploading the file");
    }

    this->sock_handler_req.reset();

    return true;
}

bool Client::get_file(char const* file)
{
    unsigned int received_packet_number = 0, packets_to_be_received;
    std::cout << file << std::endl;

    bdu::file_data_t request_dummy(this->file_handler.get_file_info(file));
    std::cout << request_dummy.file.name << std::endl; //not empty!
    this->sock_handler_req->send_packet(&request_dummy);

    this->sock_handler_req->log("Sending a file_data_t");
    auto file_data_bytes = this->sock_handler_req->wait_packet(sizeof(bdu::file_data_t));
    auto file_data = bdu::convert_from_bytes<bdu::file_data_t>(file_data_bytes.get());

    if (file_data->num_packets == 0) {
        this->log("Bad file info received");
        return false;
    }

    packets_to_be_received = file_data->num_packets;

    // Uses said number of packets to declare an array of byte_t pointers
    // pointing to the received data
    std::vector<std::unique_ptr<bdu::packet_t>> recv_file(packets_to_be_received);
    // Packet receiving loop
    for (auto& data : recv_file) {
        auto received_packet = this->sock_handler_req->wait_packet(sizeof(bdu::packet_t));

        // If the received packet is NULL, we do nothing
        if (received_packet != nullptr) {
            auto received = bdu::convert_from_bytes<bdu::packet_t>(received_packet.get());
            //TODO: Copy the received data array to the recv_file array
            data = std::move(received);
            received_packet_number++;
        }
    }

    // After receiving all packets, we send an ack with true if we received all the packets or
    // false if we didn't.
    // If the number doesnt match the expected number, the server should do something about it.
    // Also, we do nothing if the number doesnt match.
    if (received_packet_number == packets_to_be_received) {
        this->log("Success receiving the file");

        bdu::ack_t ack(true);
        this->sock_handler_req->send_packet(&ack);

        this->file_handler.write_file(file, std::move(recv_file));
    } else {
        this->log("Failure receiving the file");

        bdu::ack_t ack(false);
        this->sock_handler_req->send_packet(&ack);
    }

    this->sock_handler_req.reset();
    return true;
}

bool Client::delete_file(char const* file)
{
    if (!this->file_handler.delete_file(file)) {
        this->log("File does not exist");
        return false;
    }

    bdu::file_data_t file_data(this->file_handler.get_file_info(file));
    this->sock_handler_req->send_packet(&file_data);

    auto returned_ack = this->sock_handler_req->wait_packet(sizeof(bdu::ack_t));
    auto ack = bdu::convert_from_bytes<bdu::ack_t>(returned_ack.get());

    this->sock_handler_req.reset();

    // If it's 'false' we try again
    if (!ack->confirmation) {
        this->log("Failed deleting the file");
        return false;
    } else {
        this->log("Success deleting the file");
        return true;
    }
}

bool Client::close_session()
{
    return false;
}

bool Client::send_handshake(bdu::req request)
{
    // Sends a handshake to the server containing the request type
    bdu::handshake_t hand(request, this->userid.c_str(), this->device);
    this->sock_handler_server.send_packet(&hand);
    this->log("Sent handshake to server");

    // Waits the SYN data containing the port
    auto syn_data = this->sock_handler_server.wait_packet(sizeof(bdu::syn_t));
    auto syn = bdu::convert_from_bytes<bdu::syn_t>(syn_data.get());
    this->device = syn->device;

    // If the SYN is bad, we abort the process
    if (!syn->confirmation) {
        this->log("Received a bad SYN");
        return false;
    }

    try {
        if (!this->sock_handler_req) {
            this->sock_handler_req = std::make_unique<SocketHandler>(syn->port, this->server); 
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        this->log("Failed establishing communications with the new socket");
        return false;
    }

    this->sock_handler_req->log("Hello world!");

    return true;
}

bool Client::list_server_files()
{
    unsigned int received_packet_number = 0, packets_to_be_received;

    this->sock_handler_req->log("Sending ack!");

    bdu::ack_t hello(true);
    this->sock_handler_req->send_packet(&hello);

    this->sock_handler_req->log("Going to wait a packet!");

    auto file_data_bytes = this->sock_handler_req->wait_packet(sizeof(bdu::file_data_t));
    auto file_data = bdu::convert_from_bytes<bdu::file_data_t>(file_data_bytes.get());

    if (file_data->num_packets == 0) {
        this->log("No files on server");

        bdu::ack_t ack(true);
        this->sock_handler_req->send_packet(&ack);
        
        return true;
    }

    packets_to_be_received = file_data->num_packets;

    // Uses said number of packets to declare an array of byte_t pointers
    // pointing to the received data
    std::vector<std::unique_ptr<bdu::file_data_t>> received_file_data(packets_to_be_received);
    // Packet receiving loop
    for (auto& data : received_file_data) {
        auto received_packet = this->sock_handler_req->wait_packet(sizeof(bdu::file_data_t));

        // If the received packet is NULL, we do nothing
        if (received_packet) {
            data = bdu::convert_from_bytes<bdu::file_data_t>(received_packet.get());
            received_packet_number++;
        }
    }

    // After receiving all packets, we send an ack with true if we received all the packets or
    // false if we didn't.
    // If the number doesnt match the expected number, the server should do something about it.
    // Also, we do nothing if the number doesnt match.
    if (received_packet_number == packets_to_be_received) {
        this->log("Success receiving all file data");

        bdu::ack_t ack(true);
        this->sock_handler_req->send_packet(&ack);

        for (auto const& data : received_file_data) {
            std::cout << *data << std::endl;
        }
    } else {
        this->log("Failure receiving all file data");

        bdu::ack_t ack(false);
        this->sock_handler_req->send_packet(&ack);
    }

    this->sock_handler_req.reset();
    return true;
}

void Client::log(char const* message)
{
    printf("Client [UID: %s]: %s\n", this->userid.c_str(), message);
}
