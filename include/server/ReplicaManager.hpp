#pragma once

#include "helpers/SocketHandler.hpp"
#include "helpers/FileHandler.hpp"

#include "server/Server.hpp"

#include "util/Definitions.hpp"
#include "util/Messages.hpp"

#include <boost/filesystem.hpp>

#include <chrono>
#include <map>
#include <vector>
#include <memory>
#include <thread>

#define ALIVE_SLEEP_SECONDS 10
#define SYNC_SLEEP_SECONDS 10

namespace bf = boost::filesystem;

struct client_data_ingredients_t{
    sockaddr_in address;
    port_t port;
    // bool initialized;
    device_t device;
    char client_id[MAXNAME];
    
};

class ReplicaManager {
    SocketHandler sock_handler;
    Server server;
    FileHandler file_handler;

    std::map<id_t, sockaddr_in> group;
    std::vector<sockaddr_in> front_ends;

    id_t id = 0;

    bool primary;
    bool verbose;

    sockaddr_in primary_address;

public:
    /**
     * Runs the ReplicaManager, initiating all threads and etc.
     * 
     * @return false if something went wrong, true otherwise
     */
    bool run();

    /**
     * Factory style constructor of a ReplicaManager.
     * 
     * @return A primary ReplicaManager
     */
    static std::unique_ptr<ReplicaManager> make_primary(port_t port, bool verbose);

    /**
     * Factory style constructor of a ReplicaManager.
     * 
     * @return A backup ReplicaManager
     */
    static std::unique_ptr<ReplicaManager> make_backup(char const* host, port_t port, bool verbose);

    ReplicaManager(char const* host, port_t port, bool verbose);
    ReplicaManager(port_t port, bool verbose);

private:
    /**
     * Initiates a election following the Bully's Algorithm.
     * 
     * @return true if the object is the new leader, false if someone answered
     */
    bool election();

    void send_file(char const* file);
    void receive_file(char const* file, unsigned int packets_to_be_received);

    void sync();
    void listen();
    void check_if_alive();
    void receive_members();

    void log(char const* message);
};