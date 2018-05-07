#ifndef SERVER_HPP
#define SERVER_HPP

#define ADDR "BropDoxServer"

#include "bropdoxUtil.hpp"
#include "RequestHandler.hpp"

class Server {
public:
    int wait_client_request(int& desc);

private:
    SocketHandler* sock_handler;
    std::map<std::string,RequestHandler*> user_list;

    void init_client_sync_folder(char const* user_id);

    /**
     * Trata o handshake do cliente.
     * 
     * Esse metodo somente sera chamado na thread criada para tratar a 
     * requisicao do cliente.
     * 
     * @param package pacote de dados (handshake)
     */
    void* treat_client_request(data_buffer_t* package);

    static void* treat_helper(void* arg);

public:
    Server();
    ~Server();
};

#endif // SERVER_HPP