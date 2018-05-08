#ifndef SERVER_HPP
#define SERVER_HPP

#define ADDR "BropDoxServer"

#include "bropdoxUtil.hpp"
#include "RequestHandler.hpp"
#include "SocketHandler.hpp"

#include <map>
#include <pthread.h>

class Server {
public:
    int wait_client_request();

private:
    SocketHandler* sock_handler;
    std::map<std::string,RequestHandler*[2]> user_list;

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

typedef struct {
    Server* context;
    data_buffer_t* hand_package;
} arg_thread_t;

#endif // SERVER_HPP