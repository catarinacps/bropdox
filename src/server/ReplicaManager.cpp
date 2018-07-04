#include "server/ReplicaManager.hpp"

//Primary constructor
ReplicaManager::ReplicaManager(port_t port, bool verbose)
    : sock_handler(port+1)
    , server(port, verbose) 
    , file_handler(true)   
    , verbose(verbose)
    , primary(true)
{
}

//Backup Constructor
ReplicaManager::ReplicaManager(char const* host, port_t port, bool verbose)
    : sock_handler(port+1, gethostbyname(host), true)
    , server(port, verbose)
    , file_handler(true)
    , verbose(verbose)
    , primary(false)
    
{

}

std::unique_ptr<ReplicaManager> ReplicaManager::make_backup(char const* host, port_t port, bool verbose){
    
    auto rm = std::make_unique<ReplicaManager>(host, port,verbose);
    bdu::server_handshake_t hs;
    rm->sock_handler.send_packet(&hs);
    auto pack = rm->sock_handler.wait_packet< bdu::server_handshake_t>();

    rm->id = pack->id;

    return rm;
}

std::unique_ptr<ReplicaManager> ReplicaManager::make_primary(port_t port, bool verbose){
    
    auto rm = std::make_unique<ReplicaManager>(port,verbose);
    
    return rm;
}

bool ReplicaManager::run(){
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
    

    if(this->primary){
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
    }else{
        this->log("Creating check thread for backup server");
        std::thread alive_thread(&ReplicaManager::check_if_alive, this);

        if (!alive_thread.joinable()) {
            this->log("Failed to create new check thread...");
        }

    }    
}

void ReplicaManager::sync(){

   // auto all_files = this->server.request_handler.file_handler.get_file_info_list();  //wHY
    //FileHandler

    //send somethind to say files are coming

    for (auto& file_info : all_files) {
        this->log(file_info.file.name);
        
        //oh no 
        // auto file_data = this->sock_handler.wait_packet<bdu::file_data_t>();
        // this->log("consistency check");
        
        //if(this->sock_handler.multicast_packet()){}

        // this->send_file(file_info.file.name); FUCK
        this->log("Sent file");
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
void ReplicaManager::listen(){
    while(true){
        //should we use a RequestHandler type thing or define funcs here?
    }

}
void ReplicaManager::check_if_alive(){
    while(true){
        bdu::alive_t a;
        this->sock_handler.send_packet(&a, this->primary_address);
        auto alive = this->sock_handler.wait_packet<bdu::alive_t>();
        if(!alive){
            //eleicao
        }

        std::this_thread::sleep_for(std::chrono::seconds(ALIVE_SLEEP_SECONDS));
    }

}

void ReplicaManager::log(char const* message)
{
    printf("ReplicaManager [UID: %d]: %s\n", this->id, message);
}

void ReplicaManager::send_file(char const* file)
{
    long int file_size_in_packets = 0;
    bdu::file_info finfo;

    std::vector<std::unique_ptr<bdu::packet_t>> packets;

    
    this->log(file);
    packets = this->file_handler.read_file(file, file_size_in_packets);
    
    // If all goes well, the server sends the complete file_info to the client
    finfo = this->file_handler.get_file_info(file);
    bdu::file_data_t file_data(finfo, file_size_in_packets);
    //this->sock_handler.send_packet(&file_data);
    this->sock_handler.multicast_packet(&file_data, group);
    this->log("Sent the requested file_info");

    // Packet sending loop
    for (auto const& packet : packets) {
        this->sock_handler.multicast_packet(packet.get(),group);
        usleep(15);
    }
    this->log("Finished sending the packets");

    // The RequestHandler then procedes to wait for the Client's ack, which will contain the
    // number of packets that the client received.
    // This number of receveid packets will indicate a possible missing packet in the transmission,
    // calling for a repeat of the send_file() operation.
    //auto ack = this->sock_handler.wait_packet<bdu::ack_t>();
    
    int number_of_acks = 0;
    while(number_of_acks < group.size()){
        auto ack = this->sock_handler.wait_packet<bdu::ack_t>();
        if (ack->confirmation) {
            number_of_acks++;
        }
    }
    

    return;
}