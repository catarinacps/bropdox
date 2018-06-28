#pragma once

#include "client/Watcher.hpp"

#include "helpers/FileHandler.hpp"
#include "helpers/SocketHandler.hpp"

#include "util/Definitions.hpp"
#include "util/Exception.hpp"
#include "util/Messages.hpp"
#include "util/FileData.hpp"
#include "util/Convert.hpp"

#include <map>
#include <algorithm>
#include <memory>
#include <boost/algorithm/string.hpp>

class Client {
public:
    void command_line_interface();

private:
    device_t device = 0;
    bool logged_in;
    std::string userid;
    hostent* server;

    SocketHandler sock_handler_server;
    std::unique_ptr<SocketHandler> sock_handler_req;
    // SocketHandler sock_handler_req;
    FileHandler file_handler;

    Watcher watcher;

    // client-dev

    /**
     * Estabelece uma sessao entre o cliente e o servidor.
     * 
     * @param host A string que define o socket para o host.
     * @param port O inteiro representando a porta a ser utilizada na conexao.
     * 
     * @return O descritor da conexao ou -1 caso tenha falhado a conexao.
     */
    bool connect_to_server(char const* host, int port);

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
     * @see connect_to_server()
     */
    bool close_session();

    bool parse_input(std::vector<std::string> tokens);

    bool send_handshake(bdu::req request);

    void log(char const* message);
    
    bool list_server_files();
public:
    Client(char* uid);
};
