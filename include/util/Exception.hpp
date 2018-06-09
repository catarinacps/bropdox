#pragma once

#include <exception>

/******************************************************************************
 * Exceptions
 */

namespace bdu {

class socket_bad_bind : public std::exception {
public:
    const char* what() const noexcept
    {
        return "Bad socket bind\n";
    }
};

class socket_bad_opt : public std::exception {
public:
    const char* what() const noexcept
    {
        return "Bad setsockopt\n";
    }
};

class socket_bad_create : public std::exception {
public:
    const char* what() const noexcept
    {
        return "Bad socket creation\n";
    }
};

class not_implemented : public std::exception {
public:
    const char* what() const noexcept
    {
        return "Method not implemented\n";
    }
};

class file_does_not_exist : public std::exception {
public:
    const char* what() const noexcept
    {
        return "File does not exist\n";
    }
};
}
