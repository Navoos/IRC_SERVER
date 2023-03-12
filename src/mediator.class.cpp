#include "mediator.class.hpp"
#include "client.class.hpp"
#include "channel.class.hpp"

#include <iostream>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <string>

Mediator::Mediator(Server& server) : __server(server) {}

static bool valid_name(std::string nickname){
    if (nickname.size() > 9)
        return false;
    for (size_t i = 0; i < nickname.size(); i++){
        if (!std::isalpha(nickname[i]) && !std::isdigit(nickname[i]))
        return false;
    }
    return true;
}
void Mediator::pass_cmd(Client *client, Server server) {
    if (client->is_connected()) {
        client->put_message(ERR_ALREADYREGISTERED, ":You may not reregister");
        return;
    }

    if (client->__cmd.size() != 2) {
        client->put_message(ERR_NEEDMOREPARAMS, ":Not enough parameters");
        return;
    }
    if (client->__cmd[1] == server.get_password()) {
        client->set_accepted(true);
    }
    else {
        client->put_message(ERR_PASSWDMISMATCH, ":Password incorrect");
        return; 
    }
    client->check_connection();
    return;
}

void Mediator::user_cmd(Client *client){
    if (client->is_connected()){
        client->put_message(ERR_ALREADYREGISTERED, ":You may not reregister");
        return;
    }
    if (client->__cmd.size() < 5){
		client->put_message(ERR_NEEDMOREPARAMS, ":Not enough parameters");
        return;
    }
    client->set_username(client->__cmd[1]); 
    client->check_connection();
    return;
}

void    Mediator::nick_cmd(Client *client){
    if (client->is_connected()){
        client->put_message(ERR_RESTRICTED, ":Your connection is restricted!");
        return;
    }

	if (client->__cmd.size() != 2 || client->__cmd[1].length() == 0)
    {
		client->put_message(ERR_NONICKNAMEGIVEN, ":Not given nickname");
        return;
    }

    if (!valid_name(client->__cmd[1])){
        client->put_message(ERR_ERRONEUSNICKNAME, ":Erroneus nickname");
        return;
    } else {
        
        for (std::map<int,Client *>::iterator it = this->__clients.begin(); it != this->__clients.end(); ++it) {
            if (it->second->get_nickname() == client->__cmd[1]) {
                client->put_message(ERR_NICKNAMEINUSE, ":Nickname is already in use");
                return;
            }
        }
    }
    client->set_nickname(client->__cmd[1]);
    client->check_connection();
    return;
}

void    Mediator::delete_client(int fd) {
    this->__clients.erase(fd);
}

bool Mediator::find_client(int fd) {
    if (this->__clients.find(fd) == this->__clients.end()) {
        return false;
    }
    return true;
}

void Mediator::set_client(int fd, std::string &buffer) {
    this->__clients.at(fd)->update_client(buffer);
}

void Mediator::add_client(int fd, std::string &password, std::string &buffer, Mediator *mediator) {
    Client *client = new Client(fd, password, mediator);
    client->update_client(buffer);
    this->__clients.insert(std::make_pair(fd, client));
}

Server Mediator::get_server() {
    return this->__server;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty())
            result.push_back(item);
    }
    return result;
}

void Mediator::join_cmd(Client *client){
    if (client->__cmd.size() < 2){
        client->put_message(ERR_NEEDMOREPARAMS, ":Not enough parameters");
        return;
    }
    std::vector<std::string> channels = std::vector<std::string>();
    std::vector<std::string> keys = std::vector<std::string>();
    if (client->__cmd.size() >= 2)
        channels = split(client->__cmd[1], ',');
    if (client->__cmd.size() >= 3) {
        keys = split(client->__cmd[2], ',');
    }
    std::vector<std::string>::iterator it = channels.begin();
    int j = 0;
    for (; it != channels.end(); ++it, ++j){
        if (it->c_str()[0] != '#')
        {
            client->put_message(ERR_BADCHANMASK, ":Bad Channel Mask");
            return ;
        }else {
            if(it->size() == 1){
            std::string string = ":" + client->get_nickname() + " 480 * you need name of channel\n";
                if (send(client->get_socket(), string.c_str(), string.size(), 0) == -1){
                    perror ("send:");
                    return ;
                }
                return ;
            }
        }
        if (this->__channels.find(*it) == this->__channels.end()) {
            Channel *channel = NULL;

            if (!keys.empty() && j < (int)keys.size()) {
                channel = new Channel(*it, "", keys[j]);
            } else
                channel = new Channel(*it, "", "");
            channel->add_moderator(client->get_socket());
            channel->add_client(client);
            client->subscribe_to_channel(channel);
            this->__channels.insert(std::make_pair(*it, channel));
        } else {
            Channel *channel = this->__channels.at(*it);
            if (channel && channel->get_all_client().size() == 0) { // yaakoub add this lines
                channel->add_moderator(client->get_socket());
            }
            if (channel->find_client(client->get_socket())) {
                std::string string = ":" + client->get_nickname() + " 443 * is already on channel\n";
                if (send(client->get_socket(), string.c_str(), string.size(), 0) == -1){
                    perror ("send:");
                    return ;
                }
                return ;
            }
            if (channel->get_mode()) {
                if (channel->is_invited(client->get_socket())) {
                    if (!keys.empty() && j < (int)keys.size()) {
                        if (keys[j] == channel->get_key()) {
                                channel->add_client(client);
                                client->subscribe_to_channel(channel);
                        } else {
                            client->put_message(ERR_BADCHANNELKEY, channel->get_name() + " " + ":Cannot join channel (+k)");
                        }
                    }
                } else {
                    client->put_message(ERR_INVITEONLYCHAN, channel->get_name() + " " + ":Cannot join channel (+i)");
                }
            } else {
                    if (!keys.empty() && j < (int)keys.size()) {
                        if (!keys.empty() && j < (int)keys.size() && keys[j] == channel->get_key()) {
                                channel->add_client(client);
                                client->subscribe_to_channel(channel);
                        } else {
                            client->put_message(ERR_BADCHANNELKEY, channel->get_name() + " " + ":Cannot join channel (+k)");
                        }
                        } else {
                            channel->add_client(client);
                            client->subscribe_to_channel(channel);
                }
            }
        }
    }
}

//deadpool

std::map<std::string, Channel*> Mediator::get_channels() {
    return (this->__channels);
}

void Mediator::part_cmd(Client *client) {
    std::vector<std::string> __channels = std::vector<std::string>();
    if (client->__cmd.size() < 2) {
        client->put_message(ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
        return ;
    } else {
        __channels = split(client->__cmd[1], ',');
        if (__channels.size() == 0) {
            client->put_message("000", "PART :you should write at least one channel");
            return ;
        }
        int i = 0;
        std::string __error;
        std::string __message;
        std::string __reason;
        char        __hostname[256];
        if (gethostname(__hostname, sizeof(__hostname)) == -1)
            perror("gethostname:");
        for (std::vector<std::string>::iterator it_reason = client->__cmd.begin() + 2; it_reason != client->__cmd.end(); ++it_reason)
            __reason += *it_reason + " ";
        for (std::vector<std::string>::iterator it = __channels.begin(); it != __channels.end(); ++it, ++i) {
            if (__channels[i][0] != '#') {
                client->put_message(ERR_BADCHANMASK, "PART :Bad channel mask");
                continue ;
            } else {
                if (__channels[i][1] == '\0') {
                    __error = ":" + client->get_nickname() + " 480 * you need name of channel\n";
                    if (send(client->get_socket(), __error.c_str(), __error.size(), 0) == -1)
                        perror("send:");
                    continue ;
                }
                if (this->__channels.find(*it) == this->__channels.end()) {
                    client->put_message(ERR_NOSUCHCHANNEL, "PART :No such channel");
                    continue ;
                }
                if (client->__channels.find(*it) != client->__channels.end()) {
                    Channel *channel = client->get_channel(*it);
                    for (std::map<int, Client*>::iterator it1 = channel->get_all_client().begin(); it1 != channel->get_all_client().end(); ++it1){
                        if (__reason.size() == 0) { 
                            __message = ":" + it1->second->get_nickname() + "@" + __hostname + " PART " + __channels[i] + "    ; " + "someone" + " is leaving the channel " + __channels[i] + "\n";
                            if (send(it1->second->get_socket(), __message.c_str(), __message.size(), 0) == -1) {
                                perror("send:");
                                continue ;
                            }
                        } else {
                            __message = ":" + it1->second->get_nickname() + "@" + __hostname + " PART " + __channels[i] + "    ; " + "someone" + " is leaving the channel " + __channels[i] + " for " + __reason + "\n";
                            if (send(it1->second->get_socket(), __message.c_str(), __message.size(), 0) == -1) {
                                perror("send:");
                                continue ;
                            }
                        }
                    }
                    this->__channels.at(*it)->delete_client(client->get_socket());
                    channel->delete_moderator(client->get_socket());
                    client->__channels.erase(*it);
                    for (std::map<int, Client*>::iterator it_client = channel->get_all_client().begin(); it_client != channel->get_all_client().end(); ++it_client) {
                        if (channel->get_all_client().size() > 0 && channel->get_moderators().size() == 0)
                            channel->add_moderator(it_client->second->get_socket());
                    }
                } else {
                    client->put_message(ERR_NOTONCHANNEL, "PART :You're not on that channel");
                    continue ;
                }
            }
        }
    }
}

void Mediator::kick_cmd(Client *client) {
    std::vector<std::string> __channels = std::vector<std::string>();
    std::vector<std::string> __clients = std::vector<std::string>();
    if (client->__cmd.size() < 3){
        client->put_message(ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
        return ;
    } else {
        char        __hostname[256];
        std::string __comment;
        std::string __error;
        int         i = 0;
        if (gethostname(__hostname, sizeof(__hostname)) == -1)
            perror("gethostname");
        __channels = split(client->__cmd[1], ',');
        __clients = split(client->__cmd[2], ',');
        if (__channels.size() == 0) {
            client->put_message("001", "KICK :you should write at least one channel");
            return ;
        }
        if (__clients.size() == 0) {
            client->put_message("002", "KICK :you should write at least one user");
            return ;
        }
        for(std::vector<std::string>::iterator it_comment = client->__cmd.begin() + 3; it_comment != client->__cmd.end(); ++it_comment)
            __comment += *it_comment + " ";
        for (std::vector<std::string>::iterator it = __channels.begin(); it != __channels.end(); ++it, ++i) {
            //TODO: parse channels
            if (__channels[i][0] != '#') {
                client->put_message(ERR_BADCHANMASK, "PART :Bad channel mask");
                continue ;
            } else {
                if (__channels[i][1] == '\0') {
                    __error = ":" + client->get_nickname() + " 480 * you need name of channel\n";
                    if (send(client->get_socket(), __error.c_str(), __error.size(), 0) == -1)
                        perror("send:");
                    continue ;
                }
                if (this->__channels.find(*it) != this->__channels.end()) {
                    Channel *channel = client->get_channel(*it);
                    if (channel->find_operator(client->get_socket())) {
                        for(std::vector<std::string>::iterator it_clients = __clients.begin(); it_clients != __clients.end(); ++it_clients) {
                            std::cout << *it_clients << std::endl;
                        }
                    }
                } else {
                    client->put_message(ERR_NOSUCHCHANNEL, "PART :No such channel");
                    continue ;
                }
            }
        }
    }
}

bool    Mediator::search_channel(std::string name, std::map<std::string, Channel*>     __channels){
    if (__channels.find(name) == __channels.end()) {
        return false;
    } else {
        return true;
    }
}

void    Mediator::topic_cmd(Client *client){

    if (client->__cmd.size() < 2){
        client->put_message(ERR_NEEDMOREPARAMS, ":Not enough parameters");
        return;
    }else{
        if (client->__cmd[1][0] != '#'){
            client->put_message(ERR_BADCHANMASK, ":Bad Channel Mask");
            return ;
        }
        if(client->__cmd[1].size() == 1){
            std::string string = ":" + client->get_nickname() + " 480 * you need name of channel\n";
                if (send(client->get_socket(), string.c_str(), string.size(), 0) == -1){
                    perror ("send:");
                    return ;
                }
                return ;
            }
        if (!search_channel(client->__cmd[1], this->__channels)){
            client->put_message(ERR_NOSUCHCHANNEL, ":No such channel");
            return;
        }else{
            Channel *channel = NULL;
            channel = client->get_channel(client->__cmd[1]);
            if (channel == NULL) {
                client->put_message(ERR_NOTONCHANNEL, ":You're not on that channel");
                return;   
            }
            if (channel && !channel->find_operator(client->get_socket())){
                client->put_message(ERR_CHANOPRIVSNEEDED, ":You're not channel operator");
                return;
            }
            else {
                if (client->__cmd.size() >= 3) {
                    if (client->__cmd[2] == ":" && channel)
                        channel->set_topic("");
                    else if (channel)
                        channel->set_topic(client->__cmd[2].substr(1));
                }
            }
        }
    }
}
