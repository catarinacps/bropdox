#pragma once

#include <boost/filesystem.hpp>

#include <cstring>
#include <string>

#ifndef MAXNAME
#define MAXNAME 255
#endif

#define MAXFILES 65536

namespace bf = boost::filesystem;

/******************************************************************************
 * File data structures
 */

namespace bdu {

struct file_info {
    char name[MAXNAME * 2];
    char last_modified[MAXNAME];
    int size;
    time_t modified_time;

    file_info()
        : name{ '\0' }
        , last_modified{ '\0' }
        , size(0)
        , modified_time(0)
    {
    }

    file_info(std::string const& name_p, std::string const& sync_dir)
        : file_info()
    {
        time_t last_time;

        if (bf::exists(sync_dir + name_p)) {
            last_time = bf::last_write_time(sync_dir + name_p);
            size = bf::file_size(sync_dir + name_p);

            std::strcpy(last_modified, asctime(gmtime(&last_time)));
            modified_time = last_time;
        }

        std::strcpy(name, name_p.c_str());
    }

    friend bool operator==(file_info const& a, file_info const& b)
    {
        return 
            (std::strcmp(a.name, b.name) == 0) &&
            (std::strcmp(a.last_modified, b.last_modified)) &&
            (a.size == b.size) &&
            (a.modified_time == b.modified_time);
    }
};

struct file_data_t {
    struct file_info file;
    unsigned int num_packets;

    file_data_t(file_info const& file_p, unsigned int num_packets_p = 0)
        : file(file_p)
        , num_packets(num_packets_p)
    {
    }

    file_data_t()
        : file()
        , num_packets(0)
    {
    }

    friend bool operator==(file_data_t const& a, file_data_t const& b)
    {
        return a.file == b.file;
    }
};
}
