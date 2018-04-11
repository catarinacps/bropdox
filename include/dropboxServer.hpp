#ifndef DROPBOXSERVER_HPP
#define DROPBOXSERVER_HPP

#define ADDR "BropDoxServer"

#include <dropboxUtil.hpp>

class Server {
private:
    int sockfd;
    socklen_t client_len;
    struct sockaddr_un server_address, client_address;
    con_buffer_t buffer;

    /** Trata o handshake do cliente.
     * 
     * Esse metodo somente sera chamado na thread criada para tratar a 
     * requisicao do cliente.
     */
    void treat_client_request(con_buffer_t* buf, struct sockaddr_un* cli_addr);

public:
    Server();

    /** Sincroniza o servidor com o diretorio "sync_dir_<nomeusuario>" do cliente.
     * 
     */
    void sync_server();

    /** Recebe um arquivo file do cliente.
     * 
     * @param file Caminho completo do arquivo em questao. 
     */
    void receive_file(char* file);

    /** Envia o arquivo file para o usuario.
     * 
     * @param file O nome e extensao do arquivo em questao.
     */
    void send_file(char* file);
};

#endif // DROPBOXSERVER_HPP