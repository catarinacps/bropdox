#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>

#include "SocketHandler.hpp"
#include "FileHandler.hpp"
#include "bropdoxUtil.hpp"

class RequestHandler {
public:
    /**
     * Tells the RequestHandler to wait a confirmation ack and start handling the request.
     * 
     * @param req_type the request type (i.e. sync, send or receive).
     * @param finfo the requested or declared file_info.
     * 
     * @param a boolean representing success (true) or failure (false).
     */
    bool handle_request(req req_type);

private:
    std::string client_id;
    SocketHandler* sock_handler;
    FileHandler* file_handler;

    /**
     * Sincroniza o servidor com o diretorio "sync_dir_<nomeusuario>" do cliente.
     * 
     */
    void sync_server();

    /**
     * Recebe um arquivo file do cliente.
     * 
     * @param file Caminho completo do arquivo em questao. 
     */
    void receive_file(char const* file, unsigned int packets_to_be_received);

    /**
     * Envia o arquivo file para o usuario.
     * 
     * @param file O nome e extensao do arquivo em questao.
     */
    void send_file(char const* file);

    /**
     * Log's the message to the terminal using the UserID
     */
    void log(char const* message);

public:
    RequestHandler(sockaddr_in client_address, in_port_t port, std::string address = "");
    ~RequestHandler();
};

#endif // REQUESTHANDLER_HPP