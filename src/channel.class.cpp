#include "channel.class.hpp"
#include "client.class.hpp"

Channel::Channel(std::string name, std::string topic, std::string key){
    __name = name;
    __topic = topic;
    __key = key;
    __mode = false;
}
Channel::~Channel(){};

std::string    Channel::get_name(void) const{
    return __name;
}
std::string Channel::get_key(void) const{
    return __key;
}

std::string Channel::get_topic(void) const {
    return __topic;
}

bool Channel::get_mode(void) const{
    return __mode;
}

void Channel::set_name(std::string name){
    __name = name;
}

void Channel::set_key(std::string key){
    __key = key;
}

void Channel::set_topic(std::string topic){
    __topic = topic;
}

void Channel::set_mode(bool mode){
    __mode = mode;
}

void Channel::add_moderator(int client_id) {
    this->__moderators.insert(client_id);
}

void Channel::add_client(Client *client) {
    this->__clients.insert(std::make_pair(client->get_socket(), client));
}

bool Channel::is_invited(int client_id) {
    if (this->__invited.find(client_id) != this->__invited.end()) {
        return true;
    } else {
        return false;
    }
}