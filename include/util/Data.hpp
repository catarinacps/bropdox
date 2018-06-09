#pragma once

#include <boost/filesystem.hpp>

#include <string>

#ifndef MAXNAME
#define MAXNAME 255
#endif

#ifndef MAX_FILE_LIST_SIZE
#define MAX_FILE_LIST_SIZE 10
#endif

/******************************************************************************
 * File data structures
 */

namespace bdu {

namespace bf = boost::filesystem;

struct file_info {
    char name[MAXNAME * 2];
    char last_modified[MAXNAME];
    int size;

    file_info(std::string const& name_p, std::string const& sync_dir)
        : name{ '\0' }
        , last_modified{ '\0' }
        , size(0)
    {
        time_t last_time;

        if (bf::exists(sync_dir + name_p)) {
            last_time = bf::last_write_time(sync_dir + name_p);
            size = bf::file_size(sync_dir + name_p);
            std::strcpy(last_modified, asctime(gmtime(&last_time)));
        }

        std::strcpy(name, name_p.c_str());
    }

    file_info()
        : name{ '\0' }
        , last_modified{ '\0' }
        , size(0)
    {
    }

    bool operator<(file_info const& a) const
    {
        return name < a.name;
    }
};

struct file_data_t {
    struct file_info file;
    unsigned int num_packets;

    file_data_t(file_info const& file_p, unsigned int packets)
        : file(file_p)
        , num_packets(packets)
    {
    }

    file_data_t() = default;
};

struct file_info_list_t {
    file_data_t file_list[MAX_FILE_LIST_SIZE];
    bool has_next;
};
}
