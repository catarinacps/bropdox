#ifndef BOOST_ALL_DYN_LINK
#define BOOST_ALL_DYN_LINK
#endif

#include "bropdoxUtil.hpp"
#include <boost/filesystem.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <time.h>

namespace bf = boost::filesystem;

class FileHandler {
private:
    std::string client_id;
    bf::path syncDir;
    bool initialized = false;

public:
    FileHandler(std::string client_id);

    //eu não sei se é public ou private alguém bota no lugar certo depois
    bool write_file(char const* file_name, data_buffer_t* file_data[], int size_in_packets);

    packet_t** get_file(char const* file_name, long int& file_size);

    std::vector<file_info> get_file_info_list();

    file_info get_file_info(char const* file_name);

    // TODO: some function that gets all modified files
};