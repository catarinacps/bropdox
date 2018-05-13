#include "../include/Client.hpp"

Client::Client() {
    sockaddr_un nothing = {AF_UNIX, ""};
    this->sock_handler = new SocketHandler(nothing, ADDR);
}

int Client::login_server(char *host, int port) {
    return 0;
}

void Client::sync_client() {

}

void Client::send_file(char *file) {

}

void Client::get_file(char *file) {

}

void Client::delete_file(char *file) {

}

void Client::close_session() {

}
