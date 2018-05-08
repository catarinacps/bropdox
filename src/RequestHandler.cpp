#include "../include/RequestHandler.hpp"

RequestHandler::RequestHandler(std::string address)
{
    this->client_id = address;
    
    //! There are situations in which this constructor will fail to build 
    //! a socket because the socket will already exist.
    sock_handler = new SocketHandler(address);
}

bool RequestHandler::wait_request(req req_type, struct file_info const& finfo)
{
    data_buffer_t* data;

    data = this->sock_handler->wait_packet(sizeof(handshake_t));
    if (data == NULL) {
        return false;
    }

    // TODO: init client info/folder if necessary

    switch (req_type) {
    case req::sync: {
        this->sync_server();
    } break;
    case req::send: {
        this->send_file(finfo.name);
    } break;
    case req::receive: {
        this->receive_file(finfo.name);
    } break;
    default:
        printf("Something went wrong...\n");
        return false;
    }

    return true;
}

void RequestHandler::sync_server()
{
    //stuff
}

void RequestHandler::send_file(char const* file)
{
    //stuff
}

void RequestHandler::receive_file(char const* file)
{
    //stuff
}