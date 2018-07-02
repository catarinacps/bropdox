#pragma once

#include "helpers/SocketHandler.hpp"
#include "server/Server.hpp"

#include "util/Definitions.hpp"

#include <map>
#include <memory>
#include <thread>

class ReplicaManager {
    SocketHandler sock_handler;
    Server server;

    std::map<id_t, sockaddr_in> group;

    id_t const id = 0;

    bool primary;

private:
    /**
     * Initiates a election following the Bully's Algorithm.
     * 
     * @return true if the object is the new leader, false if someone answered
     */
    bool election();

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
    static ReplicaManager make_primary(port_t port, bool verbose);

    /**
     * Factory style constructor of a ReplicaManager.
     * 
     * @return A backup ReplicaManager
     */
    static ReplicaManager make_backup(port_t port, bool verbose);
};