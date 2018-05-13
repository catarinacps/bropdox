#include "bropdoxUtil.hpp"
#include <filesystem>

namespace fs = std::filesystem;

class FileHandler {
private:
    std::string client_id;
    fs::path syncDir;
    bool initialized = false;
    bool init_client(std::string client_id);

public:
    FileHandler(std::string client_id, bool init);

    //eu não sei se é public ou private alguém bota no lugar certo depois
    bool write_file(char const* file_name, data_buffer_t* file_data[]);
    data_buffer_t* get_file(char const* file_name);

    // TODO: some function that gets all modified files
    std::vector<data_buffer_t*> get_file_list(std::vector<char const*> file_list);
};