#include "../include/FileHandler.hpp"

namespace fs = std::filesystem;

FileHandler::FileHandler(std::string client_id, bool init) {
    syncDir = "~/sync_dir_" + client_id;
    if (!initialized) {
        initialized = init_client(client_id);
    }
}

bool FileHandler::init_client(std::string client_id) {
    if (this->client_id == client_id) {
        return true;
    } else {
        this->client_id = client_id;

        if(!fs::exists(syncDir)){
            fs::create_directory(syncDir);
        }

        //TODO: ?? alguma coisa??

        return true;
    }
}

//Function that receives an *databuffer and a filename and writes on disk
bool FileHandler::write_file(char const *file_name, data_buffer_t *file_data[]) {
    std::ofstream myFile;
    myFile.exceptions(std::ios::failbit | std::ios::badbit);
    myFile.open("~/" + *file_name, std::ios::binary | std::ios::in | std::ios::out);

    try {
        for (auto const &item : **file_data) {
            myFile << item;
        }
    } catch (std::ios::failure &e) {
        std::cerr << "Erro ao escrever arquivo.\n" << e.what();
        myFile.close();
        return false;
    }

    myFile.close();

    return true;
}

data_buffer_t *FileHandler::get_file(char const *file_name) {
    //isso aqui pega arquivo do server? hmm

    for (auto& p : fs::recursive_directory_iterator(syncDir)){
        auto accessed_file = p.path();
        if(accessed_file.filename().c_str() == file_name){
            auto buffer = new std::vector<data_buffer_t>;
            std::ifstream file;
            file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
            file.open(accessed_file);
            //TODO: pegar o conteudo e passar pra data_buffer_t[]
        }
    }

    return nullptr;
}

std::vector<data_buffer_t *> FileHandler::get_file_list(std::vector<char const *> file_list) {
    return std::vector<data_buffer_t *>();
}

