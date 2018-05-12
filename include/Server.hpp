#ifndef SERVER_HPP
#define SERVER_HPP

#define ADDR "BropDoxServer"

#include "../include/bropdoxUtil.hpp"

class Server {
public:
    int wait_client_request(int& desc);

private:
    int sockfd;
    socklen_t client_len;
    data_buffer_t buffer;
    std::vector<user_id_t> user_list;
    struct sockaddr_un server_address, client_address;

    void init_client_sync_folder(char const* user_id);

    /** Trata o handshake do cliente.
     * 
     * Esse metodo somente sera chamado na thread criada para tratar a 
     * requisicao do cliente.
     */
    void treat_client_request();

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