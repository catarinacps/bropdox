#ifndef SERVER_HPP
#define SERVER_HPP

#define ADDR "BropDoxServer"

#include "bropdoxUtil.hpp"
#include "RequestHandler.hpp"

class Server {
public:
    int wait_client_request(int& desc);

private:
    SocketHandler sock_handler;
    std::map<std::string,RequestHandler*> user_list;

    void init_client_sync_folder(char const* user_id);

    /** Trata o handshake do cliente.
     * 
     * Esse metodo somente sera chamado na thread criada para tratar a 
     * requisicao do cliente.
     */
    void* treat_client_request();

    static void* treat_helper(void* context); 

public:
    Server() : sock_handler(SocketHandler(ADDR)) {};
};

#endif // SERVER_HPP