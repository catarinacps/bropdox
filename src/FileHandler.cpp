#include "../include/FileHandler.hpp"

FileHandler::FileHandler(std::string client_id_param)
    : syncDir(std::string(getenv("HOME")) + "/sync_dir_" + client_id_param + "/"), server(false)
{
    if (client_id_param.size() <= 0) {
        throw new std::invalid_argument("Invalid ID size.");
    }

    this->client_id = client_id_param;

    try {
        if (!bf::exists(this->syncDir)) {
            bf::create_directory(this->syncDir);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw e;
    }
}

FileHandler::FileHandler(std::string client_id_param, int flag)
    : syncDir("sync_dir_" + client_id_param + "/"), server(true)
{
    if (client_id_param.size() <= 0) {
        throw new std::invalid_argument("Invalid ID size.");
    }

    this->client_id = client_id_param;

    try {
        if (!bf::exists(this->syncDir)) {
            this->log("Creating a sync_dir folder");
            bf::create_directory(this->syncDir);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw e;
    }
}

//Function that receives an *databuffer and a filename and writes on disk
bool FileHandler::write_file(char const* file_name, data_buffer_t* file_data[], int size_in_packets)
{
    std::ofstream myFile;
    std::string fname_string;

    if (std::string(file_name).find("/") == 0) {
        fname_string = this->syncDir.string() + bf::path(file_name).filename().string();
    } else {
        fname_string = this->syncDir.string() + file_name;
    }

    myFile.open(fname_string, std::ios::binary | std::ios::out);
    this->log("Opened the file");

    try {
        for (int i = 0; i < size_in_packets; i++) {
            myFile.write(reinterpret_cast<char*>(file_data[i]), PACKETSIZE);
        }
    } catch (std::ios::failure& e) {
        std::cerr << "Error while trying to write to the file:\n    " << e.what();
        myFile.close();
        return false;
    }

    this->log("Finished writing to the file");
    myFile.close();

    return true;
}

packet_t** FileHandler::get_file(char const* file_name, long int& file_size_in_packets)
{
    packet_t* read_bytes;
    packet_t** file_data;
    unsigned int i = 0;
    std::ifstream myFile;
    std::string fname_string;

    if (std::string(file_name).find("/") == 0) {
        bf::copy_file(std::string(file_name), this->syncDir.string() + bf::path(file_name).filename().string(), bf::copy_option::overwrite_if_exists);

        fname_string = this->syncDir.string() + bf::path(file_name).filename().string();
    } else {
        fname_string = this->syncDir.string() + file_name;
    }

    if (!bf::exists(fname_string)) {
        this->log("File doesn't exist");
        file_size_in_packets = 0;
        return nullptr;
    }

    myFile.open(fname_string, std::ios::binary | std::ios::in);
    this->log("Opened the file");

    try {
        file_size_in_packets = static_cast<long int>(ceil(static_cast<float>(bf::file_size(fname_string)) / static_cast<float>(PACKETSIZE)));
        file_data = new packet_t*[file_size_in_packets];
        read_bytes = new packet_t(i);

        do {
            myFile.read(reinterpret_cast<char*>(read_bytes->data), PACKETSIZE);
            file_data[i] = read_bytes;
            i++;
            read_bytes = new packet_t(i);
        } while (myFile);
        this->log("Finished reading the file");

        delete read_bytes;

        return file_data;
    } catch (std::ios::failure const& e) {
        std::cerr << "Error while trying to read the file:\n    " << e.what() << '\n';
        myFile.close();
        return nullptr;
    }
}

std::vector<file_info> FileHandler::get_file_info_list()
{
    char const* file_name;
    struct stat attrib;
    struct file_info info;
    std::vector<struct file_info> file_info_vector;

    for (auto const& p : bf::recursive_directory_iterator(this->syncDir)) {
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

file_info FileHandler::get_file_info(char const* file_name)
{
    if (std::string(file_name).find("/") == 0) {
        return file_info(bf::path(file_name).filename().string(), this->syncDir.string());
    }
    return file_info(file_name, this->syncDir.string());
}

void FileHandler::log(char const* message)
{
    printf("FileHandler [UID: %s]: %s\n",
        this->client_id.c_str(),
        message
    );
}