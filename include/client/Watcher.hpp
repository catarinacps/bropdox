#pragma once

#include "helpers/FileHandler.hpp"

#include "util/FileData.hpp"
#include "util/Exception.hpp"

#include "inotify-cpp/NotifierBuilder.h"

#include <string>
#include <thread>
#include <chrono>

using namespace inotify;

class Watcher {
    FileHandler file_handler;
    NotifierBuilder notifier;

public:
    /**
     * Runs the main watching loop.
     * 
     * @throws exception_here
     */
    void run();

private:
    /**
     * Gets called when a file gets modified.
     */
    void handle_file_modification(Notification event);

public:
    Watcher() = default;

    Watcher(std::string user_id);
};