#pragma once

#include "FileHandler.hpp"
#include "SocketHandler.hpp"
#include "bropdoxUtil.hpp"

#include <map>
#include <algorithm>
#include <boost/algorithm/string.hpp>

class Client {
public:
    void command_line_interface();

private:
    int device;
    bool logged_in;
    std::string userid;
    hostent* server;

    SocketHandler* sock_handler_server, *sock_handler_req;
    FileHandler* file_handler;

    // client-dev

    /**
     * Estabelece uma sessao entre o cliente e o servidor.
     * 
     * @param host A string que define o socket para o host.
     * @param port O inteiro representando a porta a ser utilizada na conexao.
     * 
     * @return O descritor da conexao ou -1 caso tenha falhado a conexao.
     */
    bool login_server(char const* host, int port);

    /**
     * Sincroniza o diretorio "sync_dir_<nomeusuario>" com o servidor.
     * 
     * @see Daemon de sincronizacao
     */
    bool sync_client();

    /**
     * Envia o arquivo file para o servidor
     * 
     * Essa rotina deve ser executada quando for realizar o upload de um arquivo.
     * 
     * @param file O nome e extensao do arquivo.
     */
    bool send_file(char const* file);

    /**
     * Obtem um arquivo file do servidor.
     * 
     * Essa rotina deve ser executada quando for realizar o download de um arquivo.
     * 
     * @param file O nome e extensao do arquivo.
     */
    bool get_file(char const* file);

    /**
     * Exclui um arquivo de "sync_dir_<nomeusuario>".
     * 
     * @param file O nome e extensao do arquivo.
     */
    bool delete_file(char const* file);

    /**
     * Fecha a sessao com o servidor.
     * 
     * @see login_server()
     */
    bool close_session();

    bool parse_input(std::vector<std::string> tokens);

    bool send_handshake(req request);

    void log(char const* message);

public:
    Client(char* uid);
};
