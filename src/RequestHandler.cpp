#include "../include/RequestHandler.hpp"

RequestHandler::RequestHandler(std::string address)
{
    this->client_id = address;
    
    //! There are situations in which this constructor will fail to build 
    //! a socket because the socket will already exist.
    sock_handler = new SocketHandler(address);
}

void RequestHandler::send_file(char* file)
{
    //coisas
}