#ifndef SERVER_HPP
#define SERVER_HPP

#include "RequestHandler.hpp"
#include "SocketHandler.hpp"
#include "bropdoxUtil.hpp"

#include <unordered_map>
#include <thread>

using client_data_t = std::pair<RequestHandler, unsigned int>;

class Server {
public:
    bool listen();

private:
    SocketHandler sock_handler;

    in_port_t const port;
    std::vector<bool> port_counter;

    std::unordered_map<std::string, std::vector<std::pair<RequestHandler, unsigned int>>> users;

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

    bool logout(std::string const& user_id, unsigned short int device);

    unsigned int get_next_port() noexcept;

    unsigned short int get_device(std::string const& user_id) const noexcept;

    void log(char const* userid, char const* message) const noexcept;

public:
    Server(in_port_t port);
    ~Server();
};

#endif // SERVER_HPP