#include "channel.class.hpp"

Channel::Channel(std::string name, std::string topic, std::string key, bool mode){
    __name = name;
    __topic = topic;
    __key = key;
    __mode = mode;
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