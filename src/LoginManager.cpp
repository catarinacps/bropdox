#include "../include/LoginManager.hpp"

/***************************************************************************************************
 * PUBLIC
 */

device_t LoginManager::login(std::string const& user_id)
{
    throw bdu::not_implemented();
}

bool LoginManager::logout(std::string const& user_id, device_t device)
{
    throw bdu::not_implemented();
}

client_data_t& LoginManager::get_client_data(std::string const& user_id, device_t device) const
{
    throw bdu::not_implemented();
}