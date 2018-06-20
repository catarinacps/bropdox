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
    this->running = true;

    std::thread runner([&]() { this->notifier.run(); });

    while (this->running) {
        std::this_thread::sleep_for(std::chrono::seconds(DAEMON_SLEEP_SECONDS));

        // Do a sync using the modified files queue?
    }

    this->notifier.stop();
    runner.join();
}

void Watcher::stop()
{
    this->running = false;
}

void Watcher::handle_file_modification(Notification event)
{
    // Perhaps use a modified files queue?
    std::cout << "event!!\n";
    // throw bdu::not_implemented();
}