#include "../include/FileHandler.hpp"

FileHandler::FileHandler(std::string client_id_param)
{
    this->syncDir = "~/sync_dir_" + client_id_param + "/";
    if (!this->initialized) {
        this->initialized = init_client(client_id_param);
    }
}

bool FileHandler::init_client(std::string client_id_param)
{
    if (this->client_id == client_id_param) {
        return true;
    } else {
        this->client_id = client_id_param;

        if (!fs::exists(this->syncDir)) {
            fs::create_directory(this->syncDir);
        }

        //TODO: ?? alguma coisa??

        return true;
    }
}

//Function that receives an *databuffer and a filename and writes on disk
bool FileHandler::write_file(char const* file_name, data_buffer_t* file_data[])
{
    std::ofstream myFile;
    myFile.exceptions(std::ios::failbit | std::ios::badbit);
    myFile.open(this->syncDir.string() + file_name, std::ios::binary | std::ios::in | std::ios::out);

    try {
        for (auto const& item : **file_data) {
            myFile << item;
        }
    } catch (std::ios::failure& e) {
        std::cerr << "Erro ao escrever arquivo.\n"
                  << e.what();
        myFile.close();
        return false;
    }

    myFile.close();

    return true;
}

data_buffer_t** FileHandler::get_file(char const* file_name)
{
    data_buffer_t* read_bytes;
    data_buffer_t** file_data;
    long int file_size;
    FILE* file_desc;
    int i = 0;

    for (auto const& p : fs::recursive_directory_iterator(this->syncDir)) {
        auto accessed_file = p.path();
        if (accessed_file.filename().c_str() == file_name) {
            file_desc = fopen(file_name, "rb");

            fseek(file_desc, 0L, SEEK_END);
            file_size = ftell(file_desc);
            rewind(file_desc);

            file_data = new data_buffer_t*[(int)ceil(file_size % PACKETSIZE)];
            read_bytes = new data_buffer_t[PACKETSIZE];

            while (!fread(read_bytes, 1, PACKETSIZE, file_desc)) {
                file_data[i] = read_bytes;
                read_bytes = new data_buffer_t[PACKETSIZE];
                i++;
            }

            fclose(file_desc);
            return file_data;
        }
    }

    return nullptr;

    /* 
    //isso aqui pega arquivo do server? hmm

    for (auto& p : fs::recursive_directory_iterator(syncDir)) {
        auto accessed_file = p.path();
        if (accessed_file.filename().c_str() == file_name) {
            auto buffer = new std::vector<data_buffer_t>;
            std::ifstream file;
            file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
            file.open(accessed_file);
            //TODO: pegar o conteudo e passar pra data_buffer_t[]
        }
    }

    return nullptr; */
}

std::vector<file_info> FileHandler::get_file_list()
{
    char const* file_name;
    struct stat attrib;
    struct file_info info;
    std::vector<struct file_info> file_info_vector;

    for (auto const& p : fs::recursive_directory_iterator(this->syncDir)) {
        auto accessed_file = p.path();
        file_name = accessed_file.filename().c_str();
        stat(file_name, &attrib);

        strncpy(info.name, file_name, MAXNAME * 2);
        info.name[MAXNAME * 2 - 1] = '\0';
        info.size = attrib.st_size;
        strftime(info.last_modified, MAXNAME, "%T - %d/%m/%Y", gmtime(&(attrib.st_ctime)));

        file_info_vector.push_back(info);
    }

    return file_info_vector;
}