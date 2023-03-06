#include "client.class.hpp"
#include "server.class.hpp"
#include "mediator.class.hpp"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <sys/socket.h>

bool		Client::is_connected(void) { 
    return __connected; }

bool		Client::is_accepted(void) { 
    return __accepted; }

void		Client::set_accepted(bool accepted) { 
    __accepted = accepted; }

void		Client::set_connected(bool connected) { 
    __connected = connected; }

void		Client::set_nickname(std::string nickname) { 
    __nick = nickname; }

void		Client::set_username(std::string username) { 
    __user = username; }

std::string	Client::get_nickname(void) const { return __nick; }

std::string	Client::get_username(void) const { return __user; }

int			Client::get_socket(void) const { return __fd; }

Client::Client(int fd, std::string &server_password, Mediator *mediator) : __server_password(server_password), __fd(fd), __mediator(mediator) {
    this->__connected = false;
    this->__accepted = false;
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
            this->execute(this->__mediator);
            this->__cmd.clear();
        }
        this->__buffer.erase(0, pos + 1);
        pos = this->__buffer.find("\n");
    }
}

void  Client::put_message(std::string code, std::string message)
{
    std::stringstream  msg;

    msg << ":ft_irc " << code << " " <<  get_nickname() << " " << message << "\r\n";

    if (send(get_socket(), msg.str().c_str(), msg.str().length(), 0) == -1) {
        perror("send:");
        //TODO: remove client if failed operation
    }
}


bool    Client::check_connection(void){
    if (is_connected() || !is_accepted() || get_nickname().empty() || get_username().empty())
        return false;
    set_connected(true);
    std::cout << "welcome to server";
    return true;
}

void    Client::execute(Mediator *mediator){
    if (__cmd[0] == "PASS" || __cmd[0] == "pass")
        mediator->pass_cmd(this, mediator->get_server());
    if (__cmd[0] == "USER" || __cmd[0] == "user")
        mediator->user_cmd(this);
    if (__cmd[0] == "NICK" || __cmd[0] == "nick")
        mediator->nick_cmd(this);
    if (__cmd[0] == "PART" || __cmd[0] == "part")
        mediator->part_cmd(this, mediator->get_channels(), __cmd);
}
