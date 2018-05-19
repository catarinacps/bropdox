#ifndef SERVER_HPP
#define SERVER_HPP

#include "RequestHandler.hpp"
#include "SocketHandler.hpp"
#include "bropdoxUtil.hpp"

#include <map>
#include <pthread.h>

typedef struct {
    RequestHandler* handlers[2];
    in_port_t ports[2];
} client_data_t;

class Server {
public:
    int wait_client_request();

private:
    SocketHandler* sock_handler;
    in_port_t port;
    int port_counter;
    std::map<std::string, client_data_t*> user_list;

    void init_client_sync_folder(char const* user_id);

    /**
     * Trata o handshake do cliente.
     * 
     * Esse metodo somente sera chamado na thread criada para tratar a 
     * requisicao do cliente.
     * 
     * @param package pacote de dados (handshake)
     */
    void* treat_client_request(handshake_t* package);

    bool deallocate_request_chandler(int device, std::string user_id);

    int get_next_port();

    static void* treat_helper(void* arg);

public:
    Server(in_port_t port);
    ~Server();
};

typedef struct {
    Server* context;
    handshake_t* hand_package;
} arg_thread_t;

#endif // SERVER_HPP