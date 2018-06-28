#include "client/Watcher.hpp"

Watcher::Watcher(std::string user_id)
    : file_handler(user_id)
{
    auto handle_notification = [&](Notification notification) {
        this->handle_file_modification(notification);
    };

    auto do_nothing = [](Notification notification) {};

    auto events = {
        Event::create,
        Event::modify,
        Event::remove
    };

    this->notifier = BuildNotifier()
                         .watchPathRecursively(this->file_handler.get_path())
                         .onEvents(events, handle_notification)
                         .onUnexpectedEvent(do_nothing);
}

void Watcher::run()
{
    this->running = true;

    std::thread runner([&]() { this->notifier.run(); });

    runner.detach();
}

void Watcher::stop()
{
    this->running = false;
    this->notifier.stop();
}

bool Watcher::is_running()
{
    return this->running;
}

std::vector<bdu::file_event_t> Watcher::get_events()
{
    auto copy = this->modified_files;

    this->modified_files.clear();

    return copy;
}

void Watcher::handle_file_modification(Notification event)
{
    std::cout << "event!\n";
    printf("teste\n");
    this->modified_files.emplace_back(bdu::file_info(event.path.string(), ""), event.event);
    std::cout << this->modified_files.back().file.name << " wew!\n";
    // throw bdu::not_implemented();
}