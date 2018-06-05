#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <algorithm>
#include <cmath>
#include <ctime>
#include <vector>

#include "FileHandler.hpp"
#include "SocketHandler.hpp"
#include "bropdoxUtil.hpp"

class RequestHandler {
    std::string client_id;
    SocketHandler sock_handler;
    FileHandler file_handler;
    unsigned short int device;
    unsigned int port;

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

    /**
     * Getter
     */
    unsigned short int get_device();

private:
    //TODO: Make recv, send and delete receive the whole file_info as the parameter?

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
     * Deletes the file.
     * 
     * @param file The file to be deleted.
     */
    void delete_file(char const* file);

    /**
     * Log's the message to the terminal using the UserID
     */
    void log(char const* message);

public:
    RequestHandler() {}

    RequestHandler(
        sockaddr_in client_address,
        in_port_t port_p,
        unsigned short int dev,
        std::string const& address);
};

#endif // REQUESTHANDLER_HPP