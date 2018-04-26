#include <bropdoxUtil.hpp>

class SocketHandler {
public:
    data_buffer_t* wait_packet(size_t size);

    bool send_packet(data_buffer_t& data, size_t size);

private:
    int sockfd;
    socklen_t client_len;
    struct sockaddr_un handler_address, client_address;

public:
    SocketHandler(std::string address);
};