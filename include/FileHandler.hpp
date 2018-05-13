#include "bropdoxUtil.hpp"
#include <cmath>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <time.h>

namespace fs = std::experimental::filesystem;

class FileHandler {
private:
    std::string client_id;
    fs::path syncDir;
    bool initialized = false;

    bool init_client(std::string client_id);

public:
    FileHandler(std::string client_id);

    //eu não sei se é public ou private alguém bota no lugar certo depois
    bool write_file(char const* file_name, data_buffer_t* file_data[]);

    data_buffer_t** get_file(char const* file_name);

    // TODO: some function that gets all modified files
    std::vector<file_info> get_file_list();
};