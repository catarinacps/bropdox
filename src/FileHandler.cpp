#include "../include/FileHandler.hpp"
//Function that receives an *databuffer and a filename and writes on disk

bool FileHandler::write_file(char const* file_name, data_buffer_t* file_data[])
{
    std::ofstream myFile;
    myFile.open("~/" + *file_name, std::ios::binary | std::ios::in | std::ios::out);

    for (auto const& item : **file_data){
        myFile << item;
    }   

    myFile.close();

    return true;
}

bool FileHandler::init_client(std::string client_id) {
    return false;
}

FileHandler::FileHandler(std::string client_id, bool init) {

}

data_buffer_t *FileHandler::get_file(char const *file_name) {
    return nullptr;
}

std::vector<data_buffer_t *> FileHandler::get_file_list(std::vector<char const *> file_list) {
    return std::vector<data_buffer_t *>();
}

