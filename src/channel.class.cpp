#include "../include/channel.class.hpp"

Channel::Channel(std::string name) : __name(name){}

std::string Channel::get_channel_name(){
    return __name;
}

Channel::~Channel(){}