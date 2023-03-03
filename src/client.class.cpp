#include "client.class.hpp"

// bool		Client::isConnected(void) { 
//     return __connecte; }

// void Client::put_message(std::string name, std::string message)
// {
//     std::stringstream msg;

//     msg  << ":IRC_SERVER " << name << " " << message;

//     std::cout << msg.str() << std::endl;
// }
#include <cstddef>
#include <iostream>
Client::Client(int fd) : __fd(fd) {

}

void    Client::update_client(std::string &str) {
    this->__buffer += str;
    // remove all \r from input
    size_t  pos = this->__buffer.find("\r");
    while (pos != std::string::npos) {
        this->__buffer.erase(pos);
        pos = this->__buffer.find("\r");
    }
    // parse each command at a time
    pos = this->__buffer.find("\n");
    while (pos != std::string::npos) {
        std::string cmd = this->__buffer.substr(0, pos);
        // now we have CMD ARG
        char *s = strtok((char *)cmd.c_str(), " ");
        while (s) {
            this->__cmd.push_back(std::string(s));
            s = strtok(NULL, " ");
        }
        if (!this->__cmd.empty()) {
            // HOUSSAM : execute the command here
            this->__cmd.clear();
        }
        this->__buffer.erase(0, pos + 1);
        pos = this->__buffer.find("\n");
    }
}
