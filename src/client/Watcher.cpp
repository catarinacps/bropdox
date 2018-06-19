#include "client/Watcher.hpp"

Watcher::Watcher(std::string user_id)
    : file_handler(user_id)
{
    auto handle_notification = [&](Notification notification) {
        this->handle_file_modification(notification);
    };

    auto handle_unexpected_notification = [](Notification notification) {
        std::cout << "oops\n";
    };

    auto events = {
        Event::create,
        Event::modify,
        Event::remove
    };

    this->notifier = BuildNotifier()
                         .watchPathRecursively(this->file_handler.get_path())
                         .onEvents(events, handle_notification)
                         .onUnexpectedEvent(handle_unexpected_notification);
}

void Watcher::run()
{
    std::thread runner([&]() { this->notifier.run(); });
    runner.detach();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // Do a sync using the modified files queue?
    }
}

void Watcher::handle_file_modification(Notification event)
{
    // Perhaps use a modified files queue?
    throw bdu::not_implemented();
}