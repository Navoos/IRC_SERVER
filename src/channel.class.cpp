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
    if (__mode)
        return true;
    return false;
}

bool Channel::find_client(int client_id) {
    if (this->__clients.find(client_id) == this->__clients.end()) {
        return false;
    } else {
        return true;
    }
}

bool Channel::find_operator(int client_id) {
    if (this->__moderators.find(client_id) == this->__moderators.end()) {
        return false;
    } else {
        return true;
    }
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

std::map<int, Client*>& Channel::get_all_client() {
    return this->__clients;
}

void Channel::delete_client(int client_fd) {
    if (this->__clients.find(client_fd) != this->__clients.end()) {
        this->__clients.erase(client_fd);
    }
}

void Channel::delete_moderator(int moderator) {
    if (this->__moderators.find(moderator) != this->__moderators.end())
        this->__moderators.erase(moderator);
}

std::set<int> Channel::get_moderators() {
    return (this->__moderators);
}

bool Channel::find_client(std::string &nick_name) {
    std::map<int, Client*>::iterator it;
    for (it = this->__clients.begin(); it != this->__clients.end(); ++it) {
        if (it->second->get_nickname() == nick_name) {
            return true;
        }
    }
    return false;
}

//deadpool

int Channel::get_client(std::string nick_name) {
    for (std::map<int, Client*>::iterator it = this->__clients.begin(); it != this->__clients.end(); ++it) {
        if (it->second->get_nickname() == nick_name) {
            return (it->second->get_socket());
        }
    }
    return 0;
}
