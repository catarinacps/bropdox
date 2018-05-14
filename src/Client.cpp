#include <bropdoxUtil.hpp>
#include <FileHandler.hpp>
#include "../include/Client.hpp"

Client::Client(char* uid, char *host, int port) {
    sprintf(this->userid,uid);
    if (login_server(host, port))
        this->logged_in = 1;
}

int Client::login_server(char *host, int port) {
    hostent* server = gethostbyname(host);
    if (server == NULL){
        printf("Host não encontrado.");
        return 0; //boo-hoo
    }

    sockaddr_in server_address{};
    server_address.sin_addr = *((struct in_addr*)server->h_addr);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    bzero(&(server_address.sin_zero), 8);

    std::string login_str;
    login_str = "BDX/01-";
    login_str += this->userid;

    convert_helper_t login_pkt = convert_to_data(login_str);

    sock_handler->send_packet(login_pkt.pointer, login_pkt.size);

    return 1;
}

void Client::sync_client() {

}

void Client::send_file(char *file) {

}

void Client::get_file(char *file) {
    file_info fileInfo;
    strcpy(fileInfo.name,file);
    fileInfo.size = 0;
    strcpy(fileInfo.last_modified,"");

    handshake_t handshake;
    strcpy(handshake.userid,this->userid);
    handshake.req_type = req::send;
    handshake.file = fileInfo;
    handshake.num_packets = 0;

    convert_helper_t package = convert_to_data(handshake);
    this->sock_handler->send_packet(package.pointer, package.size);

    data_buffer_t* data_buffer;
    data_buffer = this->sock_handler->wait_packet(sizeof(syn_t));
}

void Client::delete_file(char *file) {

}

void Client::close_session() {

}
