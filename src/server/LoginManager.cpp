#include "server/LoginManager.hpp"

/***************************************************************************************************
 * PUBLIC
 */

device_t LoginManager::login(std::string const& user_id, sockaddr_in const& user_address, port_t port)
{
    this->m_login.lock();

    auto const device = this->reserve_device(user_id);

    this->m_login.unlock();

    RequestHandler rh(user_address, port, device, user_id);

    this->m_map.lock();

    this->users.at(user_id).at(device - 1) = client_data_t(std::move(rh), port);

    this->m_map.unlock();

    this->get_client_data(user_id, device).initialized = true;

    return device;
}

bool LoginManager::logout(std::string const& user_id, device_t device)
{
    if (device > MAX_CONCURRENT_DEV || device == 0) {
        throw std::invalid_argument("Server::login : invalid device argument");
    }

    this->m_map.lock();

    this->users.at(user_id).at(device) = client_data_t();

    this->m_map.unlock();
    return true;
}

client_data_t& LoginManager::get_client_data(std::string const& user_id, device_t device)
{
    return this->users[user_id].at(device - 1);
}

device_t LoginManager::reserve_device(std::string const& user_id)
{
    auto i = 0;
    for (auto& item : this->users[user_id]) {
        if (!item.initialized) {
            item.initialized = true;
            return i + 1;
        }
        i++;
    }

    throw std::domain_error("LoginManager::reserve_device : too many devices logged in");
}