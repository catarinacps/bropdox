#include "../include/RequestHandler.hpp"

RequestHandler::RequestHandler(std::string address)
{
    this->client_id = address;
    sock_handler = SocketHandler(address);
}

void RequestHandler::send_file(char* file)
{
    //coisas
}