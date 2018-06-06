#ifndef SERVER_HPP
#define SERVER_HPP

#include "RequestHandler.hpp"
#include "SocketHandler.hpp"
#include "bropdoxUtil.hpp"

#include <array>
#include <thread>
#include <mutex>
#include <unordered_map>

using device_t = unsigned short int;

struct client_data_t {
    RequestHandler handler;
    unsigned int port;
    bool initialized;

    client_data_t()
        : initialized(false)
    {
    }

    client_data_t(RequestHandler&& rh, unsigned int port_p)
        : handler(std::move(rh))
        , port(port_p)
    {
    }

    client_data_t& operator=(client_data_t&& move)
    {
        this->handler = std::move(move.handler);
        this->port = std::move(move.port);
        this->initialized = std::move(move.initialized);

        return *this;
    }
};

class Server {
    SocketHandler mutable sock_handler;    
    in_port_t const port;

    std::vector<bool> port_counter;
    std::unordered_map<std::string, std::array<client_data_t, MAX_CONCURRENT_USERS>> users;

    std::mutex mutable m_login;
    std::mutex mutable m_map;

public:
    /**
     * Sets the server to wait the next handshake package from a client.
     * 
     * @return success or failure on receiving said handshake
     */
    bool listen();

private:
    /***********************************************************************************
     * CORE
     */

    /**
     * Treats the client handshake.
     * 
     * Gets called on the creation of a treater thread 
     * 
     * @param package the handshake
     */
    void treat_client_request(std::unique_ptr<handshake_t> package);

    /**
     * Tries to login the given user_id.
     * 
     * @param user_id the UserID
     * 
     * @return the new user device
     */
    device_t login(std::string const& user_id);

    /**
     * Tries to logout the given user_id.
     * 
     * @param user_id the UserID
     * @param device the current user device
     * 
     * @return success or failure
     */
    bool logout(std::string const& user_id, device_t device);

    /***********************************************************************************
     * HELPER
     */

    bool verify_login(std::string const& user_id, device_t device) const noexcept;

    device_t treat_client_login(std::string const& user_id);

    in_port_t reserve_port() noexcept;

    device_t reserve_device(std::string const& user_id);

    void log(char const* userid, char const* message) const noexcept;

public:
    Server(in_port_t port);
};

#endif // SERVER_HPP