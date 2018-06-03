#ifndef SERVER_HPP
#define SERVER_HPP

#include "RequestHandler.hpp"
#include "SocketHandler.hpp"
#include "bropdoxUtil.hpp"

#include <unordered_map>
#include <array>
#include <thread>

using client_data_t = std::pair<std::unique_ptr<RequestHandler>, unsigned int>;

/* typedef struct {
    RequestHandler* handlers[2];
    in_port_t ports[2];
} client_data_t; */

class Server {
public:
    bool listen();

private:
    in_port_t const port;
    std::vector<bool> port_counter;
    
    SocketHandler sock_handler;

    std::unordered_map<std::string, std::array<client_data_t, MAX_CONCURRENT_USERS>> users;
    //std::unordered_map<std::string, client_data_t*> users;

    // server-dev

    /**
     * Trata o handshake do cliente.
     * 
     * Esse metodo somente sera chamado na thread criada para tratar a 
     * requisicao do cliente.
     * 
     * @param package pacote de dados (handshake)
     */
    void treat_client_request(std::unique_ptr<handshake_t> package);

    std::pair<client_data_t, unsigned short> login(std::string const& user_id, unsigned short int device);

    bool logout(std::string const& user_id, unsigned short int const& device);

    unsigned int get_next_port() noexcept;

    unsigned short int get_device(std::string const& user_id) const noexcept;

    void log(char const* userid, char const* message) const noexcept;

public:
    Server(in_port_t port);
    ~Server();
};

#endif // SERVER_HPP