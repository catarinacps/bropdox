#include "server/ReplicaManager.hpp"

//Primary constructor
ReplicaManager::ReplicaManager(port_t port, bool verbose_p)
    : sock_handler(port + 1)
    , server(port, this->sock_handler.get_own_address(), verbose_p)
    , file_handler(true)
    , primary(true)
    , verbose(verbose_p)
{
}

//Backup Constructor
ReplicaManager::ReplicaManager(char const* host, port_t port, bool verbose_p)
    : sock_handler(port + 1, gethostbyname(host), true)
    , server(port, this->sock_handler.get_own_address(), verbose_p)
    , file_handler(true)
    , primary(false)
    , verbose(verbose_p)
{
}

std::unique_ptr<ReplicaManager> ReplicaManager::make_backup(char const* host, port_t port, bool verbose)
{
    auto rm = std::make_unique<ReplicaManager>(host, port, verbose);

    bdu::rm_operation_t request(bdu::serv_req::request_entrance);
    rm->sock_handler.send_packet(&request);

    auto pack = rm->sock_handler.wait_packet<bdu::rm_syn_t>();

    if (pack) {
        rm->id = pack->id;
    } else {
        rm.reset();

        return nullptr;
    }

    rm->receive_members();

    bdu::ack_t ack(true);
    rm->sock_handler.send_packet(&ack);

    return rm;
}

std::unique_ptr<ReplicaManager> ReplicaManager::make_primary(port_t port, bool verbose)
{
    return std::make_unique<ReplicaManager>(port, verbose);
}

void ReplicaManager::receive_members()
{
    auto member = this->sock_handler.wait_packet<bdu::member_t>();

    while (member && (member->id != 0)) {
        this->group[member->id] = member->address;

        member = this->sock_handler.wait_packet<bdu::member_t>();
    }
}

bool ReplicaManager::run()
{
    //TODO: 4 threads
    //primary listen to clients
    //primary sync
    //All threads listen to other RM
    //Backups send vivo?
    this->log("Creating server listening thread");
    std::thread rm_listen_thread(&ReplicaManager::listen, this);

    if (!rm_listen_thread.joinable()) {
        this->log("Failed to create new server listen thread...");
        return false;
    }

    if (this->primary) {

        this->log("Creating syncing thread for primary server");
        std::thread syncing_thread([&]() {
            while (true) {
                this->sync();

                std::this_thread::sleep_for(std::chrono::seconds(SYNC_SLEEP_SECONDS));
            }
        });

        if (!syncing_thread.joinable()) {
            this->log("Failed to create new syncing thread...");
            return false;
        }

        syncing_thread.detach();
    } else {
        this->log("Creating check thread for backup server");
        std::thread alive_thread(&ReplicaManager::check_if_alive, this);

        if (!alive_thread.joinable()) {
            this->log("Failed to create new check thread...");
            return false;
        }

        alive_thread.detach();
    }

    rm_listen_thread.detach();

    this->server.listen();

    return true;
}

void ReplicaManager::sync()
{
    bdu::rm_operation_t sync_op(bdu::serv_req::sync);
    this->sock_handler.multicast_packet(&sync_op, this->group);

    for (auto const& entry : bf::directory_iterator(".")) {
        auto path = entry.path();

        if (bf::is_directory(path)) {
            auto directory_name = path.filename().string();
            auto client_name = directory_name.substr(directory_name.find_last_of('_'), directory_name.size());

            bdu::client_t client(client_name);
            this->sock_handler.multicast_packet(&client, this->group);

            this->file_handler.set_sync_dir(path.string());
            auto all_files = this->file_handler.get_file_info_list();

            for (auto const& file_info : all_files) {
                this->sock_handler.multicast_packet(&file_info, this->group);

                this->send_file(file_info.file.name);
                this->log("Sent file");
            }

            bdu::file_data_t empty;
            this->sock_handler.multicast_packet(&empty, this->group);
        }
    }

    bdu::client_t client("");
    this->sock_handler.multicast_packet(&client, this->group);

    this->log("Finished syncing files");
    return;
}

void ReplicaManager::listen()
{
    while (true) {
        //should we use a RequestHandler type thing or define funcs here?
        auto request = this->sock_handler.wait_packet<bdu::rm_operation_t>();
        auto address = this->sock_handler.get_last_address();

        switch (request->req) {
        case bdu::serv_req::alive: {
            if (this->primary) {
                bdu::rm_operation_t ok(bdu::serv_req::alive);
                this->sock_handler.send_packet(&ok, address);
            }
            break;
        }
        case bdu::serv_req::new_member: {
            auto member = this->sock_handler.wait_packet<bdu::member_t>();
            this->group[member->id] = member->address;
            break;
        }
        case bdu::serv_req::election: {
            //TODO: this
            break;
        }
        case bdu::serv_req::request_entrance: {
            if (this->primary) {
                //TODO: PLEASE
                //DOnt you FOrgEt AbouT mE
                //logica de id!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                bdu::rm_syn_t new_id(this->group.size() + 1);
                this->sock_handler.send_packet(&new_id, address);

                for (auto const& member : this->group) {
                    bdu::member_t existing_member(member.first, member.second);
                    this->sock_handler.send_packet(&existing_member, address);
                }

                bdu::member_t empty(0, sockaddr_in());
                this->sock_handler.send_packet(&empty, address);

                auto new_member_ack = this->sock_handler.wait_packet<bdu::ack_t>();
                if (!new_member_ack) {
                    this->log("Aborting new member addition...");
                    return;
                }

                bdu::rm_operation_t new_member_operation(bdu::serv_req::new_member);
                this->sock_handler.multicast_packet(&new_member_operation, this->group);

                usleep(10);

                bdu::member_t new_member_address(id, address);
                this->sock_handler.multicast_packet(&new_member_address, this->group);

                this->group[new_id.id] = address;
            }
            break;
        }
        case bdu::serv_req::sync: {
            if (!this->primary) {
                auto client = this->sock_handler.wait_packet<bdu::client_t>();

                while (client && (std::strlen(client->name) > 0)) {
                    auto sync_dir = std::string("sync_dir") + client->name + "/";
                    if (!bf::exists(sync_dir)) {
                        bf::create_directory(sync_dir);
                    }

                    this->file_handler.set_sync_dir(sync_dir);

                    auto file_info = this->sock_handler.wait_packet<bdu::file_data_t>();
                    do {
                        this->receive_file(file_info->file.name, file_info->num_packets);

                        file_info = this->sock_handler.wait_packet<bdu::file_data_t>();
                    } while (file_info && (file_info->num_packets != 0));

                    client = this->sock_handler.wait_packet<bdu::client_t>();
                }

                this->file_handler.set_sync_dir("./");
            }
            break;
        }
        case bdu::serv_req::client_login: {
            auto hand = this->sock_handler.wait_packet<bdu::handshake_t>();
            if (this->primary) {
                if (hand->req_type == bdu::req::fe) {
                    this->front_ends.push_back(hand->fe_address);

                    bdu::address_t my_address(this->sock_handler.get_own_address());
                    this->sock_handler.send_packet(&my_address, hand->fe_address);

                    bdu::rm_operation_t propagate_fe(bdu::serv_req::new_front_end);
                    this->sock_handler.multicast_packet(&propagate_fe, this->group);

                    usleep(10);

                    this->sock_handler.multicast_packet(hand.get(), this->group);
                } else if (hand->req_type == bdu::req::login) {
                    bdu::rm_operation_t propagate_login(bdu::serv_req::propagate_login);
                    this->sock_handler.multicast_packet(&propagate_login, this->group);

                    usleep(10);

                    this->sock_handler.multicast_packet(hand.get(), this->group);
                }
            } else {
                auto dummy_syn = this->sock_handler.wait_packet<bdu::syn_t>();
            }
            break;
        }
        case bdu::serv_req::propagate_login: {
            if (!this->primary) {
                auto hand = this->sock_handler.wait_packet<bdu::handshake_t>();

                this->sock_handler.send_packet(hand.get(), this->server.get_own_address());
            }
            break;
        }
        case bdu::serv_req::new_front_end: {
            if (!this->primary) {
                auto hand = this->sock_handler.wait_packet<bdu::handshake_t>();

                this->front_ends.push_back(hand->fe_address);
            }
            break;
        }
        }
    }
}

void ReplicaManager::check_if_alive()
{
    while (true) {
        /* bdu::alive_t are_u_alive;
        this->sock_handler.send_packet(&are_u_alive, this->primary_address);

        auto alive = this->sock_handler.wait_packet<bdu::alive_t>();

        if (!alive->alive) {
            //eleicao
        } */

        std::this_thread::sleep_for(std::chrono::seconds(ALIVE_SLEEP_SECONDS));
    }
}

void ReplicaManager::receive_file(char const* file, unsigned int const packets_to_be_received)
{
    unsigned int received_packet_number = 0;

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

        this->file_handler.write_file(file, std::move(recv_file));
    } else {
        this->log("Failure receiving the file");
    }

    return;
}

void ReplicaManager::send_file(char const* file)
{
    long int file_size_in_packets = 0;
    bdu::file_info finfo;

    this->log(file);
    auto packets = this->file_handler.read_file(file, file_size_in_packets);

    // If all goes well, the server sends the complete file_info to the client
    finfo = this->file_handler.get_file_info(file);
    bdu::file_data_t file_data(finfo, file_size_in_packets);

    this->sock_handler.multicast_packet(&file_data, this->group);
    this->log("Sent the requested file_info");

    // Packet sending loop
    for (auto const& packet : packets) {
        this->sock_handler.multicast_packet(packet.get(), this->group);
        usleep(15);
    }
    this->log("Finished sending the packets");

    /* unsigned int number_of_acks = 0;
    while (number_of_acks < this->group.size()) {
        auto ack = this->sock_handler.wait_packet<bdu::ack_t>();
        if (ack->confirmation) {
            number_of_acks++;
        }
    } */

    return;
}

void ReplicaManager::log(char const* message)
{
    if (this->verbose) {
        printf("ReplicaManager [UID: %d]: %s\n", this->id, message);
    }
}
