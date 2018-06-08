#pragma once

#include "bropdoxUtil.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <time.h>

class FileHandler {
public:
    //eu não sei se é public ou private alguém bota no lugar certo depois
    bool write_file(char const* file_name, std::vector<std::unique_ptr<packet_t>> file_data) const;

    packet_t** get_file(char const* file_name, long int& file_size);

    bool delete_file(char const* file_name) const;

    std::vector<file_info> get_file_info_list() const;

    file_info get_file_info(char const* file_name) const;

private:
    std::string client_id;
    bf::path syncDir;

    void log(char const* message) const;

public:
    FileHandler(std::string client_id);

    FileHandler(std::string client_id, int flag);

    FileHandler() {}

    // TODO: some function that gets all modified files
};