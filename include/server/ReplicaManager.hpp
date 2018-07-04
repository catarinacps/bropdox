#pragma once

#include "helpers/SocketHandler.hpp"
#include "server/Server.hpp"

#include "util/Definitions.hpp"
#include "util/Messages.hpp"
#include <chrono>
#include <map>
#include <memory>
#include <thread>

#define ALIVE_SLEEP_SECONDS 10

class ReplicaManager {
    SocketHandler sock_handler;
    Server server;
    FileHandler file_handler;

    std::map<id_t, sockaddr_in> group;

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

    void log(char const* message);
    void send_file(char const* file);

    void sync();
    void listen();
    void check_if_alive();
};