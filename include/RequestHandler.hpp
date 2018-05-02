#include "bropdoxUtil.hpp"

class RequestHandler {
public:
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

private:
    std::string client_id;
    SocketHandler sock_handler;

public:
    RequestHandler(std::string address);
};