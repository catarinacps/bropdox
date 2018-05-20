#include "bropdoxUtil.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <time.h>

class FileHandler {
public:
    //eu não sei se é public ou private alguém bota no lugar certo depois
    bool write_file(char const* file_name, data_buffer_t* file_data[], int size_in_packets);

    packet_t** get_file(char const* file_name, long int& file_size);

    std::vector<file_info> get_file_info_list();

    file_info get_file_info(char const* file_name);

private:
    std::string client_id;
    bf::path syncDir;
    bool server;

    void log(char const* message);

public:
    FileHandler(std::string client_id);
    FileHandler(std::string client_id, int flag);


    // TODO: some function that gets all modified files
};