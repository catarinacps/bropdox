#ifndef SERVER_HPP
#define SERVER_HPP

#include "RequestHandler.hpp"
#include "SocketHandler.hpp"
#include "bropdoxUtil.hpp"

#include <map>
#include <thread>

typedef struct {
    RequestHandler* handlers[2];
    in_port_t ports[2];
} client_data_t;

class Server {
public:
    bool listen();

private:
    in_port_t port;
    int port_counter;
    SocketHandler* sock_handler;
    std::map<std::string, client_data_t*> user_list;

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

    bool logout_client(int device, std::string user_id);

    void log(char const* userid, char const* message) const;

    int get_next_port();

public:
    Server(in_port_t port);
    ~Server();
};

#endif // SERVER_HPP