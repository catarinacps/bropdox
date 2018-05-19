#include "../include/Client.hpp"

Client::Client(char* uid, char* host, int port)
{
    sprintf(this->userid, uid);
    if (login_server(host, port))
        this->logged_in = 1;

    this->file_handler = new FileHandler(this->userid);
}

int Client::login_server(char* host, int port)
{


}

void Client::sync_client()
{
}

void Client::send_file(char* file)
{
}

void Client::get_file(char* file)
{
    /* file_info fileInfo;
    strcpy(fileInfo.name, file);
    fileInfo.size = 0;
    strcpy(fileInfo.last_modified, "");

    handshake_t handshake;
    strcpy(handshake.userid, this->userid);
    handshake.req_type = req::send;
    handshake.file = fileInfo;
    handshake.num_packets = 0;

    convert_helper_t hs_package = convert_to_data(handshake);
    this->sock_handler->send_packet(hs_package.pointer, hs_package.size);

    data_buffer_t* data_buffer;
    data_buffer = this->sock_handler->wait_packet(sizeof(syn_t));

    syn_t* syn_package = convert_to_syn(data_buffer);
    fileInfo.size = syn_package->file_size;

    data_buffer_t* recv_file[syn_package->num_packets];
    data_buffer_t* recv_buffer;
    packet_t* recv_packet = NULL;
    for (int i = 0; i < syn_package->num_packets; ++i) {
        recv_buffer = this->sock_handler->wait_packet(sizeof(packet_t));

        if (recv_buffer != NULL) {
            recv_packet = convert_to_packet(recv_buffer);
            recv_file[recv_packet->num] = recv_packet->data;
            delete recv_packet;
        }

        delete[] recv_buffer;
    }

    this->file_handler->write_file(fileInfo.name, recv_file, syn_package->num_packets); */
}

void Client::delete_file(char* file)
{
}

void Client::close_session()
{
}
