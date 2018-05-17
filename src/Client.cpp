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
    int sockfd, n;
	unsigned int length;
	struct sockaddr_in serv_addr, from, server_address;
    handshake_t hand;
    struct file_info finfo;

    char bufferf[256];

    hostent* server = gethostbyname(host);
    if (server == NULL) {
        printf("Host não encontrado.");
        return 0; //boo-hoo
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("ERROR opening socket");

    server_address.sin_addr = *((struct in_addr*)server->h_addr);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    bzero(&(server_address.sin_zero), 8);

    hand.req_type = req::send;
    hand.file = finfo;
    hand.num_packets = 0;
    std::fill_n(hand.userid, MAXNAME, 'A');
    hand.userid[254] = '\0';
   
    n = sendto(sockfd, &hand, sizeof(handshake_t), 0, (const struct sockaddr *) &server_address, sizeof(struct sockaddr_in));
	
    if(n<0){
        printf("ERROR sendto");
        return -1;
    }
	
	length = sizeof(struct sockaddr_in);
	n = recvfrom(sockfd, bufferf, 256, 0, (struct sockaddr *) &from, &length);
	if (n < 0)
		printf("ERROR recvfrom");

	printf("Got an ack: %s\n", bufferf);
	
	close(sockfd);

    return 0;

}

void Client::sync_client()
{
}

void Client::send_file(char* file)
{
}

void Client::get_file(char* file)
{
    file_info fileInfo;
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

        delete recv_buffer;
    }

    this->file_handler->write_file(fileInfo.name, recv_file, syn_package->num_packets);
}

void Client::delete_file(char* file)
{
}

void Client::close_session()
{
}
