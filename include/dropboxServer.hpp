#ifndef DROPBOXSERVER_H
#define DROPBOXSERVER_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dropboxUtil.h>

class Server {
private:
    //
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

#endif // DROPBOXSERVER_H