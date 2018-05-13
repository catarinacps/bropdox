#include "../include/FileHandler.hpp"
//Function that receives an *databuffer and a filename and writes on disk

bool write_file(char const* file_name, data_buffer_t* file_data)
{
    ofstream file;
    file.open("~/" + filename, ios::binary | ios::in | ios::out);

    for (auto const& item : file_data){
        file.write((char*) &file_data, sizeof(file_data));
    }   

    file.close();
}