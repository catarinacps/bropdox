#include "server/ReplicaManager.hpp"

//Primary constructor
ReplicaManager::ReplicaManager(port_t port, bool verbose)
    : sock_handler(port + 1)
    , server(port, verbose)
    , file_handler(true)
    , verbose(verbose)
    , primary(true)
{
}

//Backup Constructor
ReplicaManager::ReplicaManager(char const* host, port_t port, bool verbose)
    : sock_handler(port + 1, gethostbyname(host), true)
    , server(port, verbose)
    , file_handler(true)
    , verbose(verbose)
    , primary(false)

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

    return rm;
}

std::unique_ptr<ReplicaManager> ReplicaManager::make_primary(port_t port, bool verbose)
{
    return std::make_unique<ReplicaManager>(port, verbose);
}

bool ReplicaManager::run()
{
    //TODO: 4 threads
    //primary listen to clients
    //primary sync
    //All threads listen to other RM
    //Backups send vivo?
    this->log("Creating server listening thread");
    std::thread server_listen_thread(&ReplicaManager::listen, this);

    if (!server_listen_thread.joinable()) {
        this->log("Failed to create new server listen thread...");
    }

    if (this->primary) {
        this->log("Creating  client listening thread for primary server");
        std::thread client_listen_thread(&Server::listen, &(this->server));

        if (!client_listen_thread.joinable()) {
            this->log("Failed to create new client listen thread...");
        }

        this->log("Creating syncing thread for primary server");
        std::thread syncing_thread(&ReplicaManager::sync, this);

        if (!syncing_thread.joinable()) {
            this->log("Failed to create new syncing thread...");
        }

        client_listen_thread.detach();
        syncing_thread.detach();
    } else {
        this->log("Creating check thread for backup server");
        std::thread alive_thread(&ReplicaManager::check_if_alive, this);

        if (!alive_thread.joinable()) {
            this->log("Failed to create new check thread...");
        }

        alive_thread.detach();
    }

    server_listen_thread.detach();
}

void ReplicaManager::sync()
{
    for (auto const& entry : bf::directory_iterator(".")) {
        auto path = entry.path();

        if (bf::is_directory(path)) {
            auto directory_name = path.filename().string();
            auto client_name = directory_name.substr(directory_name.find_last_of('_'), directory_name.size());

            bdu::client_t client(client_name);
            this->sock_handler.multicast_packet(&client, this->group);

            this->file_handler.set_sync_dir(path);
            auto all_files = this->file_handler.get_file_info_list();

            for (auto const& file_info : all_files) {
                this->sock_handler.multicast_packet(file_info, this->group);

                this->send_file(file_info.file.name);
                this->log("Sent file");
            }

            bdu::file_data_t empty;
            this->sock_handler.multicast_packet(&empty);
        }
    }

    /* 
    auto ack = this->sock_handler.wait_packet<bdu::ack_t>();
    if (ack){
        if(!ack->confirmation) {
            this->log("Syncing failure");
            return;
        }
    }else{
        this->log("No ack recieved");
    }
 */

    this->log("Finished syncing");
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
                auto id = this->group.size() + 1;

                bdu::rm_syn_t new_id(id);
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

                bdu::member_t new_member_address(id, address);
                this->sock_handler.multicast_packet(&new_member_address, this->group);
            }
            break;
        }
        case bdu::serv_req::sync: {
            //
        }
        }
    }
}

void ReplicaManager::check_if_alive()
{
    while (true) {
        bdu::alive_t are_u_alive;
        this->sock_handler.send_packet(&are_u_alive, this->primary_address);

        auto alive = this->sock_handler.wait_packet<bdu::alive_t>();

        if (!alive->alive) {
            //eleicao
        }

        std::this_thread::sleep_for(std::chrono::seconds(ALIVE_SLEEP_SECONDS));
    }
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
    //this->sock_handler.send_packet(&file_data);
    this->sock_handler.multicast_packet(&file_data, group);
    this->log("Sent the requested file_info");

    // Packet sending loop
    for (auto const& packet : packets) {
        this->sock_handler.multicast_packet(packet.get(), group);
        usleep(15);
    }
    this->log("Finished sending the packets");

    unsigned int number_of_acks = 0;
    while (number_of_acks < group.size()) {
        auto ack = this->sock_handler.wait_packet<bdu::ack_t>();
        if (ack->confirmation) {
            number_of_acks++;
        }
    }

    return;
}

void ReplicaManager::log(char const* message)
{
    printf("ReplicaManager [UID: %d]: %s\n", this->id, message);
}
