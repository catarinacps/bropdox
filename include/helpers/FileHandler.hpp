#pragma once

#include "util/Definitions.hpp"
#include "util/Messages.hpp"
#include "util/FileData.hpp"
#include "util/Exception.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <time.h>

class FileHandler {
    std::string client_id;
    bf::path syncDir;

public:
    //eu não sei se é public ou private alguém bota no lugar certo depois
    bool write_file(char const* file_name, std::vector<std::unique_ptr<bdu::packet_t>> file_data, std::string path = std::string() ) const;

    std::vector<std::unique_ptr<bdu::packet_t>> read_file(char const* file_name, long int& file_size);

    bool delete_file(char const* file_name) const;

    bool copy_file_to_sync_dir(char const* file_name) const;

    bool check_freshness(bdu::file_info const& file) const;

    std::vector<bdu::file_data_t> get_file_info_list() const;

    bdu::file_info get_file_info(char const* file_name) const;

    bf::path get_path() const;

private:
    void log(char const* message) const;

public:
    FileHandler(std::string client_id);

    FileHandler(std::string client_id, int flag);

    FileHandler() {}

    FileHandler(bool flag);

    // TODO: some function that gets all modified files
};