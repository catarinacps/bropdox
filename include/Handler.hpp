#include <bropdoxUtil.hpp>

class Handler {
    bool write_file(char const* file_name);

    data_buffer_t* get_file(char const* file_name);

    // TODO: some function that gets all modified files
    std::vector<data_buffer_t*> get_file_list(std::vector<char const*> file_list);
private:
    std::string id;

    bool init_client(std::string client_id);
public:
    Handler(std::string client_id, bool init);
    //
};