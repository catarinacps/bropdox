#ifndef SERVER_HPP
#define SERVER_HPP

#define ADDR "BropDoxServer"

#include <bropdoxUtil.hpp>

class Server {
private:
    int sockfd;
    socklen_t client_len;
    data_buffer_t buffer;
    struct sockaddr_un server_address, client_address;

    /** Trata o handshake do cliente.
     * 
     * Esse metodo somente sera chamado na thread criada para tratar a 
     * requisicao do cliente.
     */
    void treat_client_request(data_buffer_t* buf, struct sockaddr_un* cli_addr);

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

public:
    Server();
};

#endif // SERVER_HPP