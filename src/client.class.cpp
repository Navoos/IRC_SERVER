#include "client.class.hpp"
#include "server.class.hpp"
#include "mediator.class.hpp"
#include "channel.class.hpp"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/param.h>

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
    this->__voice = false;
}

void    Client::update_client(std::string &str) {
    this->__buffer += str;
    // remove all \r from input
    size_t  pos = this->__buffer.find("\r");
    while (pos != std::string::npos) {
        this->__buffer.erase(this->__buffer.begin() + pos);
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

bool Client::has_voice() {
    return this->__voice;
}

bool  Client::put_message(std::string message)
{
    std::stringstream  msg;
    // if (get_nickname().size() == 0)
    //     msg << ":ft_irc " << code << " " <<  "*" << " " << message << "\r\n";
    // else
        msg << message << "\r\n";
        

    if (send(get_socket(), msg.str().c_str(), msg.str().length(), 0) == -1) {
        perror("send:");
        return (false);
    }
    return (true);
}

bool    Client::check_connection(void){
    if ( get_nickname().empty() || get_username().empty() || is_connected() || !is_accepted())
        return false;
    set_connected(true);
    char hostname[MAXHOSTNAMELEN];
    memset(hostname, 0, sizeof hostname);
    if (gethostname(hostname, MAXHOSTNAMELEN) == -1) {
        perror("gethotname");
        put_message(":ft_irc 001 " + get_nickname() +  " :Welcome to the Internet Relay Network, " + __nick + "\n");
        // put_message(RPL_WELCOME, ":Welcome to the Internet Relay Network, " + __nick + "\n");
    }
    else 
        put_message(":ft_irc 001 " + get_nickname() +  " :Welcome to the Internet Relay Network, " + __nick + " [ ! " + __user + "@" + hostname + "]\n");
        // put_message(RPL_WELCOME, ":Welcome to the Internet Relay Network, " + __nick + " [ ! " + __user + "@" + hostname + "]\n");
    return true;
}

void Client::subscribe_to_channel(Channel *channel) {
    this->__channels.insert(std::make_pair(channel->get_name(), channel));
}

Channel* Client::get_channel(std::string &name) {
    if (this->__channels.find(name) == this->__channels.end()) {
        return NULL;
    } else {
        return this->__channels.at(name);
    }
}

void Client::erase_channel(std::string &channel)
{
    if (this->__channels.find(channel) != this->__channels.end())
        this->__channels.erase(channel);
}

// void Client::erase_channel(std::string channel) {
//     if (this->__channels.find(channel) != this->__channels.end())
//         this->__channels.erase(channel);
// }

// std::map<std::string, Channel*> Client::get_channels() {
//     return __channels;
// }

void   Client::execute(Mediator *mediator){
    if (__cmd[0] == "PASS" || __cmd[0] == "pass")
        mediator->pass_cmd(this, mediator->get_server());
    else if (__cmd[0] == "USER" || __cmd[0] == "user")
        mediator->user_cmd(this);
    else if (__cmd[0] == "NICK" || __cmd[0] == "nick")
        mediator->nick_cmd(this);
    else if (__cmd[0] == "JOIN" || __cmd[0] == "join")
        mediator->join_cmd(this);
    else if (__cmd[0] == "TOPIC" || __cmd[0] == "topic")
        mediator->topic_cmd(this);
    //deadpool
    else if (__cmd[0] == "PART" || __cmd[0] == "part")
        mediator->part_cmd(this);
    else if (__cmd[0] == "KICK" || __cmd[0] == "kick")
        mediator->kick_cmd(this);
    if (__cmd[0] == "TOPIC" || __cmd[0] == "topic")
        mediator->topic_cmd(this);
    if (__cmd[0] == "invite" || __cmd[0] == "INVITE")
        mediator->invite_cmd(this);
    if (__cmd[0] == "privmsg" || __cmd[0] == "PRIVMSG")
        mediator->privmsg_cmd(this);
    // //deadpool
    // if (__cmd[0] == "PART" || __cmd[0] == "part")
    //     mediator->part_cmd(this, __cmd);
    else if (__cmd[0] == "MODE" || __cmd[0] == "mode")
        mediator->mode_cmd(this);
    else if (__cmd[0] == "QUIT" || __cmd[0] == "quit")
        mediator->quit_cmd(this);
    else
        mediator->command_not_found(this);
}
