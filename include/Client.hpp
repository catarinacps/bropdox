#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "RequestHandler.hpp"
#include "SocketHandler.hpp"
#include "bropdoxUtil.hpp"

#include <map>
#include <pthread.h>

class Client {
private:
    int logged_in;
    int devices[2];
    data_buffer_t buffer;
    char userid[MAXNAME];
    socklen_t server_len;
    struct file_info files[MAXFILES];

public:
    Client();

    /**
     * Estabelece uma sessao entre o cliente e o servidor.
     * 
     * @param host A string que define o socket para o host.
     * @param port O inteiro representando a porta a ser utilizada na conexao.
     * 
     * @return O descritor da conexao ou -1 caso tenha falhado a conexao.
     */
    int login_server(char* host, int port);

    /**
     * Sincroniza o diretorio "sync_dir_<nomeusuario>" com o servidor.
     * 
     * @see Daemon de sincronizacao
     */
    void sync_client();

    /**
     * Envia o arquivo file para o servidor
     * 
     * Essa rotina deve ser executada quando for realizar o upload de um arquivo.
     * 
     * @param file O nome e extensao do arquivo.
     */
    void send_file(char* file);

    /**
     * Obtem um arquivo file do servidor.
     * 
     * Essa rotina deve ser executada quando for realizar o download de um arquivo.
     * 
     * @param file O nome e extensao do arquivo.
     */
    void get_file(char* file);

    /**
     * Exclui um arquivo de "sync_dir_<nomeusuario>".
     * 
     * @param file O nome e extensao do arquivo.
     */
    void delete_file(char* file);

    /**
     * Fecha a sessao com o servidor.
     * 
     * @see login_server()
     */
    void close_session();
};

#endif // CLIENT_HPP