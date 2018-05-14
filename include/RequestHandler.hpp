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
    bool wait_request(req req_type, struct file_info const& finfo = file_info());

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
    void receive_file(char const* file);

    /**
     * Envia o arquivo file para o usuario.
     * 
     * @param file O nome e extensao do arquivo em questao.
     */
    void send_file(char const* file);

public:
    RequestHandler(sockaddr_un client_address, std::string address = "");
};

#endif // REQUESTHANDLER_HPP