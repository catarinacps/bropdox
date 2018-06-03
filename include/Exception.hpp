#include <exception>

/******************************************************************************
 * Exceptions
 */

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
