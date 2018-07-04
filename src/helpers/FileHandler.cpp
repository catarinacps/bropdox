#include "helpers/FileHandler.hpp"

FileHandler::FileHandler(std::string client_id_param)
    : syncDir(std::string(getenv("HOME")) + "/sync_dir_" + client_id_param + "/")
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
    : syncDir("sync_dir_" + client_id_param + "/")
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

FileHandler::FileHandler(bool flag)
    : syncDir("./")
{
    this->client_id = "Replica Manager";
}

//Function that receives an *databuffer and a filename and writes on disk
bool FileHandler::write_file(char const* file_name, std::vector<std::unique_ptr<bdu::packet_t>> file_data, std::string path) const
{
    std::ofstream myFile;
    std::string fname_string;

    if (std::string(file_name).find("/") == 0) {
        fname_string = this->syncDir.string() + bf::path(file_name).filename().string();
    } else if(!path.empty()) {
        fname_string = path + "/" +file_name;
    } else {
        fname_string = this->syncDir.string() + file_name;
    }

    myFile.open(fname_string, std::ios::binary | std::ios::out);
    this->log("Opened the file");

    try {
        for (auto const& packet : file_data) {
            myFile.write(reinterpret_cast<char*>(packet->data), packet->data_size);
        }
    } catch (std::ios::failure& e) {
        std::cerr << "Error while trying to write to the file:\n"
                  << e.what();
        bf::remove(fname_string);
        myFile.close();
        return false;
    }

    this->log("Finished writing to the file");
    myFile.close();

    return true;
}

std::vector<std::unique_ptr<bdu::packet_t>> FileHandler::read_file(char const* file_name, long int& file_size_in_packets)
{
    std::ifstream myFile;
    std::string fname_string;

    std::string file_name_str(file_name);

    if (file_name_str.find("/") != 0) {
        file_name_str = this->syncDir.string() + file_name_str;
    }

    if (!bf::exists(file_name_str)) {
        this->log("File doesn't exist");
        file_size_in_packets = 0;
        throw bdu::file_does_not_exist();
    }

    myFile.open(file_name_str, std::ios::binary | std::ios::in);
    this->log("Opened the file");

    auto file_size = bf::file_size(file_name_str);
    file_size_in_packets = static_cast<long int>(ceil(static_cast<float>(file_size) / static_cast<float>(PACKETSIZE)));
    std::vector<std::unique_ptr<bdu::packet_t>> data;

    unsigned int i = 0;
    auto packet = std::make_unique<bdu::packet_t>(i, (file_size > PACKETSIZE) ? PACKETSIZE : file_size);

    do {
        myFile.read(reinterpret_cast<char*>(packet->data), PACKETSIZE);
        file_size -= PACKETSIZE;
        data.push_back(std::move(packet));
        i++;
        packet = std::make_unique<bdu::packet_t>(i, (file_size > PACKETSIZE) ? PACKETSIZE : file_size);
    } while (myFile);
    this->log("Finished reading the file");

    return data;
}

std::vector<bdu::file_data_t> FileHandler::get_file_info_list() const
{
    std::vector<bdu::file_data_t> file_info_vector;

    for (auto const& p : bf::recursive_directory_iterator(this->syncDir)) {
        auto accessed_file = p.path();
        auto file_name = accessed_file.filename().string();

        bdu::file_info finfo(file_name, this->syncDir.string());
        auto file_size = static_cast<unsigned int>(ceil(static_cast<float>(finfo.size) / static_cast<float>(PACKETSIZE)));
        bdu::file_data_t file_data(finfo, file_size);

        file_info_vector.push_back(file_data);
    }

    return file_info_vector;
}

bool FileHandler::delete_file(char const* file_name) const
{
    if (bf::exists(this->syncDir.string() + file_name)) {
        bf::remove(this->syncDir.string() + file_name);
        return true;
    }

    return false;
}

bool FileHandler::copy_file_to_sync_dir(char const* file_name) const
{
    try {
        bf::copy_file(std::string(file_name), this->syncDir.string() + bf::path(file_name).filename().string(), bf::copy_option::overwrite_if_exists);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return false;
    }

    return true;
}

bool FileHandler::check_freshness(bdu::file_info const& file) const
{
    //FIXME: Handle conflicts?

    auto my_file = this->get_file_info(file.name);

    return file.modified_time > my_file.modified_time;
}

bdu::file_info FileHandler::get_file_info(char const* file_name) const
{
    if (std::string(file_name).find("/") == 0) {
        return bdu::file_info(bf::path(file_name).filename().string(), this->syncDir.string());
    }
    return bdu::file_info(file_name, this->syncDir.string());
}

bf::path FileHandler::get_path() const
{
    return this->syncDir;
}

void FileHandler::log(char const* message) const
{
    printf("FileHandler [UID: %s]: %s\n",
        this->client_id.c_str(),
        message);
}
