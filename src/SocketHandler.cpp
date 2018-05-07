#include "../include/SocketHandler.hpp"

SocketHandler::SocketHandler(std::string address)
{
    this->sockfd = init_unix_socket(this->handler_address, address.c_str());
    this->client_len = sizeof(struct sockaddr_un);

    if (bind(this->sockfd, (struct sockaddr*)&(this->handler_address), sizeof(struct sockaddr)) < 0) {
        printf("Error while binding the socket, please try again...\n");
    }
}

data_buffer_t* SocketHandler::wait_packet(size_t size)
{
    data_buffer_t* buffer = new data_buffer_t[size];

    int desc = recvfrom(this->sockfd, (void*)buffer, size, 0, (struct sockaddr*)&(this->client_address), &(this->client_len));
    if (desc < 0) {
        printf("Error while receiving packet...\n\n");
        delete buffer;
        return NULL;
    }

    //! Caller must delete this object later
    return buffer;
}

bool SocketHandler::send_packet(data_buffer_t& data, size_t size)
{
    int desc = sendto(this->sockfd, &data, size, 0, (struct sockaddr*)&(this->client_address), sizeof(struct sockaddr));
    if (desc < 0) {
        printf("Error while sending packet...\n\n");
        return false;
    }

    return true;
}

SocketHandler::~SocketHandler()
{
    close(this->sockfd);
}