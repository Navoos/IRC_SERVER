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
        client->put_message(":ft_irc 462 " + client->get_nickname() +" :You may not reregister");
        // client->put_message(ERR_ALREADYREGISTERED, ":You may not reregister");
        return;
    }

    if (client->__cmd.size() != 2) {
        client->put_message(":ft_irc 461 " + client->get_nickname() +" :Not enough parameters");
        // client->put_message(ERR_NEEDMOREPARAMS, ":Not enough parameters");
        return;
    }
    if (client->__cmd[1] == server.get_password()) {
        client->set_accepted(true);
    }
    else {
        client->put_message(":ft_irc 464 " + client->get_nickname() +" :Password incorrect");
        // client->put_message(ERR_PASSWDMISMATCH, ":Password incorrect");
        return; 
    }
    client->check_connection();
    return;
}

void Mediator::user_cmd(Client *client){
    if (client->is_connected()){
        client->put_message(":ft_irc 462 " + client->get_nickname() +" :You may not reregister");
        return;
    }
    if (client->__cmd.size() < 5){
		client->put_message(":ft_irc 461 " + client->get_nickname() +" :Not enough parameters");
        return;
    }
    client->set_username(client->__cmd[1]); 
    client->check_connection();
    return;
}

void    Mediator::nick_cmd(Client *client){
    if (client->is_connected()){
        client->put_message(":ft_irc 018 " + client->get_nickname() +" :Your connection is restricted!");
        // client->put_message(ERR_RESTRICTED, ":Your connection is restricted!");
        return;
    }

	if (client->__cmd.size() != 2 || client->__cmd[1].length() == 0)
    {
        client->put_message(":ft_irc 431 " + client->get_nickname() +" :Not given nickname");
		// client->put_message(ERR_NONICKNAMEGIVEN, ":Not given nickname");
        return;
    }

    if (!valid_name(client->__cmd[1])){
        client->put_message(":ft_irc 432 " + client->get_nickname() +" :Erroneus nickname");
        // client->put_message(ERR_ERRONEUSNICKNAME, ":Erroneus nickname");
        return;
    } else {
        
        for (std::map<int,Client *>::iterator it = this->__clients.begin(); it != this->__clients.end(); ++it) {
            if (it->second->get_nickname() == client->__cmd[1]) {
                client->put_message(":ft_irc 433 " + client->get_nickname() +" :Nickname is already in use");
                // client->put_message(ERR_NICKNAMEINUSE, ":Nickname is already in use");
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
        client->put_message(":ft_irc 461 " + client->get_nickname() +" :Not enough parameters");
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
            client->put_message(":ft_irc 476 " + client->get_nickname() +" :Bad Channel Mask");
            return ;
        }else {
            if(it->size() == 1){
                client->put_message(":ft_irc 480 " + client->get_nickname() +" :you need name of channel");
                return ;
            }
        }
        if (this->__channels.find(*it) == this->__channels.end()) {
            Channel *channel = NULL;
            if (!keys.empty() && j < (int)keys.size()) {
                channel = new Channel(*it, "", keys[j]);
                channel->set_key(keys[j]);
            } else
                channel = new Channel(*it, "", "");
            channel->set_modeinvite(false);
            channel->set_modetopic(false);
            channel->add_moderator(client->get_socket());
            channel->add_client(client);
            client->subscribe_to_channel(channel);
            this->__channels.insert(std::make_pair(*it, channel));
            client->put_message(":ft_irc 400 " + client->get_nickname() +" join this "+ channel->get_name() + " :not topic");
        } else {
            Channel *channel = this->__channels.at(*it);
            if (channel && channel->get_all_client().size() == 0) {
                channel->add_moderator(client->get_socket());
            }
            if (channel && channel->get_all_client().size() == 0) {
                channel->add_moderator(client->get_socket());
            }
            if (channel->find_client(client->get_socket())) {
                client->put_message(":ft_irc 480 " + client->get_nickname() +" :is already on channel");
                return ;
            }
            if (channel->get_mode()) {
                if (channel->is_invited(client->get_socket())) {
                    if (!keys.empty() && j < (int)keys.size()) {
                        if (keys[j] == channel->get_key()) {
                                channel->add_client(client);
                                client->subscribe_to_channel(channel);
                                client->put_message(":ft_irc 400 " + client->get_nickname() +" join this "+ channel->get_name() + " :not topic");
                        } else {
                            client->put_message(":ft_irc 475 " + client->get_nickname() + " " + channel->get_name() + " :Cannot join channel (+k)");
                        }
                    }
                } else {
                    client->put_message(":ft_irc 473 " + client->get_nickname() + " " + channel->get_name() + " :Cannot join channel (+i)");
                }
            } else {
                    if (!channel->get_key().empty()) {
                        if (!keys.empty() && j < (int)keys.size() && keys[j] == channel->get_key()) {
                                channel->add_client(client);
                                client->subscribe_to_channel(channel);
                                client->put_message(":ft_irc 400 " + client->get_nickname() +" join this "+ channel->get_name() + " :not topic");
                        } else {
                            client->put_message(":ft_irc 475 " + client->get_nickname() + " " + channel->get_name() + " :Cannot join channel (+k)");
                        }
                        } else {
                            channel->add_client(client);
                            client->subscribe_to_channel(channel);
                            client->put_message(":ft_irc 400 " + client->get_nickname() +" join this "+ channel->get_name() + " :not topic");
                }
            }
        }
    }
}

//deadpool

std::map<std::string, Channel*>& Mediator::get_channels() {
    return (this->__channels);
}

void Mediator::part_cmd(Client *client) {
    std::vector<std::string>    __channels = std::vector<std::string>();
    std::string                 error;
    std::string                 message;
    std::string                 reason;

    if (client->__cmd.size() < 2) {
        error = ":ft_irc 461 " + client->get_nickname() + " PART :Not enough parameters.";
        client->put_message(error);
        return ;
    } else {
        __channels = split(client->__cmd[1], ',');
        if (__channels.size() == 0) {
            error = ":ft_irc 000 " + client->get_nickname() + " PART :you should write at least one channel.";
            client->put_message(error);
            return ;
        }
        int i = 0;
        for (std::vector<std::string>::iterator it_reason = client->__cmd.begin() + 2; it_reason != client->__cmd.end(); ++it_reason)
            reason += *it_reason + " ";
        for (std::vector<std::string>::iterator it = __channels.begin(); it != __channels.end(); ++it, ++i) {
            if (__channels[i][0] != '#') {
                error = ":ft_irc 476 " + client->get_nickname() + " PART :Bad channel mask.";
                client->put_message(error);
                continue ;
            } else {
                if (__channels[i][1] == '\0') {
                    error = ":ft_irc 002 " + client->get_nickname() + " PART :you need name of channel.";
                    client->put_message(error);
                    continue ;
                }
                if (this->__channels.find(*it) == this->__channels.end()) {
                    error = ":ft_irc 403 " + client->get_nickname() + " PART :No such channel.";
                    client->put_message(error);
                    continue ;
                }
                if (client->__channels.find(*it) != client->__channels.end()) {
                    Channel *channel = client->get_channel(*it);
                    if (!channel)
                        return ;
                    for (std::map<int, Client*>::iterator it1 = channel->get_all_client().begin(); it1 != channel->get_all_client().end(); ++it1){
                        if (reason.size() == 0) { 
                            message = ":ft_irc " + it1->second->get_nickname() + " :" + client->get_nickname() + " is leaving the channel " + __channels[i] + ".";
                            it1->second->put_message(message);
                            continue ;
                        } else {
                            message = ":ft_irc " + it1->second->get_nickname() + " :" + client->get_nickname() + " is leaving the channel " + __channels[i] + " for " + reason + ".";
                            it1->second->put_message(message);
                            continue ;
                        }
                    }
                    this->__channels.at(*it)->delete_client(client->get_socket());
                    client->__channels.erase(*it);
                    if (channel->find_operator(client->get_socket())) {
                        channel->delete_moderator(client->get_socket());
                        for (std::map<int, Client*>::iterator it_client = channel->get_all_client().begin(); it_client != channel->get_all_client().end(); ++it_client) {
                            if (channel->get_all_client().size() > 0 && channel->get_moderators().size() == 0) {
                                channel->add_moderator(it_client->second->get_socket());
                                message = ":ft_irc :" + it_client->second->get_nickname() + " now is the operator of channel " + __channels[i] + ".";
                                it_client->second->put_message(message);
                            }
                        }
                    }
                } else {
                    error = ":ft_irc 441 " + client->get_nickname() + " PART :You're not on that channel.";
                    client->put_message(error);
                    continue ;
                }
            }
        }
    }
}

void Mediator::kick_cmd(Client *client) {
    std::vector<std::string>    __channels = std::vector<std::string>();
    std::vector<std::string>    clients = std::vector<std::string>();
    std::string                 comment;
    std::string                 message;
    std::string                 error;
    char                        hostname[256];

    if (client->__cmd.size() < 3){
        error = ":ft_irc 461 " + client->get_nickname() + " KICK :Not enough parameters.";
        client->put_message(error);
        return ;
    } else {
        int         i = 0;
        if (gethostname(hostname, sizeof(hostname)) == -1)
            perror("gethostname");
        __channels = split(client->__cmd[1], ',');
        clients = split(client->__cmd[2], ',');
        if (__channels.size() == 0) {
            error = ":ft_irc 000 " + client->get_nickname() + " KICK :you should write at least one channel.";
            client->put_message(error);
            return ;
        }
        if (clients.size() == 0) {
            error = ":ft_irc 001 " + client->get_nickname() + " KICK :you should write at least one user.";
            client->put_message(error);
            return ;
        }
        for(std::vector<std::string>::iterator it_comment = client->__cmd.begin() + 3; it_comment != client->__cmd.end(); ++it_comment) {
            if (clients[3][0] == ':')
                comment += *it_comment + " ";
            else {
                error = ":ft_irc 002 " + client->get_nickname() + " KICK :you should write \':\' before the reason.";
                client->put_message(error);
                return ;
            }
        }
        for (std::vector<std::string>::iterator it = __channels.begin(); it != __channels.end(); ++it, ++i) {
            Channel *channel = client->get_channel(*it);
            if (__channels[i][0] != '#') {
                error = ":ft_irc 476 " + *it + " :Bad channel mask";
                client->put_message(error);
                continue ;
            } else {
                if (__channels[i][1] == '\0') {
                    error = ":ft_irc 002 " + client->get_nickname() + " :You need name of channel.";
                    client->put_message(error);
                    continue ;
                }
                if (this->__channels.find(*it) != this->__channels.end()) {
                    if (!channel) {
                        error = ":ft_irc 442 " + client->get_nickname() + " " + *it + " :You're not on that channel.";
                        client->put_message(error);
                        continue ;
                    }
                    if (channel->find_client(client->get_socket())) {
                        if (channel->find_operator(client->get_socket())) {
                            for(std::vector<std::string>::iterator it_clients = clients.begin(); it_clients != clients.end(); ++it_clients) {
                                if (channel->find_client(*it_clients)) {
                                    Client *__client = this->get_client(*it_clients);
                                    if (!__client)
                                        return;
                                    for (std::map<int, Client*>::iterator it = channel->get_all_client().begin(); it != channel->get_all_client().end(); ++it) {
                                        if (comment.size() == 0) {
                                            message = ":ft_irc " + client->get_nickname() + " :Kick " + *it_clients + " from " + __channels[i] + ".";
                                        } else {
                                            message = ":ft_irc " + client->get_nickname() + " :Kick " + *it_clients + " from " + __channels[i] + " using \"" + comment + "\" as the reason.";
                                        }
                                        it->second->put_message(message);
                                    }
                                    __client->erase_channel(*it);
                                    this->__channels.at(*it)->delete_client(channel->get_client(*it_clients));
                                    if (channel->find_operator(client->get_socket())) {
                                        if (*it_clients == client->get_nickname())
                                            channel->delete_moderator(client->get_socket());
                                        for (std::map<int, Client*>::iterator it_client = channel->get_all_client().begin(); it_client != channel->get_all_client().end(); ++it_client) {
                                            if (channel->get_all_client().size() > 0 && channel->get_moderators().size() == 0) {
                                                channel->add_moderator(it_client->second->get_socket());
                                                message = ":ft_irc    ; Command to kick " + it_client->second->get_nickname() + " is the new operator of channel " + __channels[i] + ".";
                                                it_client->second->put_message(message);
                                            }
                                        }
                                    }
                                } else {
                                    error = ":ft_irc 441 " + client->get_nickname() + " " + *it_clients + " " + channel->get_name() + " :They aren't on that channel.";
                                    client->put_message(error);
                                    continue ;
                                }
                            }
                        } else {
                            error = ":ft_irc 482 " + client->get_nickname() + " " + channel->get_name() + " :You're not channel operator.";
                            client->put_message(error);
                            continue ;
                        }
                    } else {
                        error = ":ft_irc 442 " + client->get_nickname() + " " + channel->get_name() + " :You're not on that channel.";
                        client->put_message(error);
                        continue ;
                    }
                } else {
                    error = ":ft_irc 403 " + client->get_nickname() + " " + *it + " :No such channel.";
                    client->put_message(error);
                    continue ;
                }
            }
        }
    }
}

void    Mediator::mode_cmd(Client *client) {
    std::string     error;
    std::string     message;
    std::string     modestring;
    std::string     key;
    std::string     user;
    std::string     target;

    if (client->__cmd.size() < 3) {
        error = ":ft_irc 461 " + client->get_nickname() + " MODE :Not enough parameters.";
        client->put_message(error);
        return ;
    }
    target = client->__cmd[1];
    modestring = client->__cmd[2];
    if (client->__cmd.size() == 4) {
        key = client->__cmd[3];
        user = client->__cmd[3];
    }
    if (target[0] == '#') {
        if (!this->search_channel(target, this->__channels)) {
            error = ":ft_irc 403 " + client->get_nickname() + " " + target + " :No such channel.";
            client->put_message(error);
            return ;
        }
        Channel *channel = client->get_channel(target);
        if (!channel) {
            error = ":ft_irc 442 " + client->get_nickname() + " " + target + " :You're not on that channel.";
            client->put_message(error);
            return ;
        }
        if (channel->find_client(client->get_socket())) {
            if (channel->find_operator(client->get_socket())) {
                if (this->__channels.find(target) != this->__channels.end()) {
                    if (modestring[0] == '+') {
                        if (modestring[1] == 'i') {
                            channel->set_modeinvite(true);
                        } else if (modestring[1] == 't') {
                            channel->set_modetopic(true);
                        } else if (modestring[1] == 'k') {
                            if (client->__cmd.size() == 4) {
                                channel->set_modekey(true);
                                channel->set_key(key);
                                message = ":ft_irc " + channel->get_name() + " +k :Set the channel key to " + key + ".";
                                client->put_message(message);
                                return ;
                            } else {
                                error = ":ft_irc 472 " + client->get_nickname() + " +k :Is unknown mode char to me.";
                                client->put_message(error);
                                return ;
                            }
                        } else if (modestring[1] == 'o') {
                                if (!user.empty()) {
                                    if (channel->find_client(user)) {
                                        if (!channel->find_operator(channel->get_client(user))) {
                                            channel->add_moderator(channel->get_client(user));
                                            for (std::map<int, Client*>::iterator it_client = channel->get_all_client().begin(); it_client != channel->get_all_client().end(); it_client++) {
                                                message = ":ft_irc " + client->get_nickname() + " +o :Set the operator to " + user + ".";
                                                it_client->second->put_message(message);
                                            }
                                        } else {
                                            error = ":ft_irc 000 " + target + " :Is a operator.";
                                            client->put_message(error);
                                            return ;
                                        }
                                    } else {
                                        error = ":ft_irc 401 " + client->get_nickname() + " " + target + " :No such nick/channel.";
                                        client->put_message(error);
                                        return ;
                                    }
                                } else {
                                    error = ":ft_irc 002 :You should write a client.";
                                    client->put_message(error);
                                    return ;
                                }
                            } else {
                            error = ":ft_irc 501 " + client->get_nickname() + " :Unknown MODE flag.";
                            client->put_message(error);
                            return ;
                        }
                    } else if (modestring[0] == '-') {
                        if (modestring[1] == 'i') {
                            channel->set_modeinvite(false);
                        } else if (modestring[1] == 't') {
                            channel->set_modetopic(false);
                        } else if (modestring[1] == 'k') {
                            if (!channel->get_key().empty()) {
                                message = ":ft_irc " + channel->get_name() + " -k :Remove the channel key from " + channel->get_name() + ".";
                                client->put_message(message);
                            }
                            channel->set_modekey(false);
                            channel->set_key("");
                            return ;
                        } else if (modestring[1] == 'o') {
                                if (!user.empty()) {
                                    if (channel->find_client(user)) {
                                        if (channel->find_operator(channel->get_client(user))) {
                                            channel->delete_moderator(channel->get_client(user));
                                            for (std::map<int, Client*>::iterator it_client = channel->get_all_client().begin(); it_client != channel->get_all_client().end(); it_client++) {
                                                message = ":ft_irc " + client->get_nickname() + " -o :Remove the operator from " + user + ".";
                                                it_client->second->put_message(message);
                                            }
                                        } else {
                                            error = ":ft_irc 000 " + user + " :Is not a operator.";
                                            client->put_message(error);
                                            return ;
                                        }
                                    } else {
                                        error = ":ft_irc 401 " + client->get_nickname() + " " + user + " :No such nick/channel.";
                                        client->put_message(error);
                                        return ;
                                    }
                                } else {
                                    error = ":ft_irc 002 :You should write a client.";
                                    client->put_message(error);
                                    return ;
                                }
                            } else {
                            error = ":ft_irc 501 " + client->get_nickname() + " :Unknown MODE flag.";
                            client->put_message(error);
                            return ;
                        }
                    } else {
                        error = ":ft_irc 501 " + client->get_nickname() + " :Unknown MODE flag.";
                        client->put_message(error);
                        return ;
                    }
                }
            } else {
                error = ":ft_irc 482 " + client->get_nickname() + " " + channel->get_name() + " :You're not channel operator.";
                client->put_message(error);
                return ;
            }
        }
    } else {
        for (std::map<std::string, Channel*>::iterator it = this->get_channels().begin(); it != this->get_channels().end(); it++) {
            if (modestring[1] == 'o') {
                if (it->second->find_client(client->get_socket())) {
                    if (it->second->find_operator(client->get_socket())) {
                        if (it->second->find_client(target)) {
                            if (modestring == "+o") {
                                if (!it->second->find_operator(it->second->get_client(target))) {
                                    it->second->add_moderator(it->second->get_client(target));
                                    for (std::map<int, Client*>::iterator it_client = it->second->get_all_client().begin(); it_client != it->second->get_all_client().end(); it_client++) {
                                        message = ":ft_irc " + client->get_nickname() + " +o :Set the operator to " + target + ".";
                                        it_client->second->put_message(message);
                                    }
                                } else {
                                    error = ":ft_irc 000 " + target + " :Is a operator.";
                                    client->put_message(error);
                                    return ;
                                }
                            } else if (modestring == "-o") {
                                if (it->second->find_operator(it->second->get_client(target))) {
                                    it->second->delete_moderator(it->second->get_client(target));
                                    for (std::map<int, Client*>::iterator it_client = it->second->get_all_client().begin(); it_client != it->second->get_all_client().end(); it_client++) {
                                        message = ":ft_irc " + client->get_nickname() + " -o :Remove the operator from " + target + ".";
                                        it_client->second->put_message(message);
                                    }
                                } else {
                                    error = ":ft_irc 001 " + target + " :Is not a operator.";
                                    client->put_message(error);
                                    return ;
                                }
                            } else {
                                error = ":ft_irc 501 " + client->get_nickname() + " :Unknown MODE flag.";
                                client->put_message(error);
                                return ;
                            }
                        } else {
                            error = ":ft_irc 401 " + client->get_nickname() + " " + target + " :No such nick/channel.";
                            client->put_message(error);
                            return ;
                        }
                    } else {
                        error = ":ft_irc 482 " + client->get_nickname() + " " + it->second->get_name() + " :You're not channel operator.";
                        client->put_message(error);
                        return ;
                    }
                } else {
                    error = ":ft_irc 442 " + client->get_nickname() + " " + it->second->get_name() + " :You're not on that channel.";
                    client->put_message(error);
                    return ;
                }
            } else {
                error = ":ft_irc 501 " + client->get_nickname() + " :Unknown MODE flag.";
                client->put_message(error);
                return ;
            }
        }
    }
}

// TODO: yaakoub dir shi tawil m3a had lqlawi dial quit
void    Mediator::quit_cmd(Client *client) 
{
    std::string reason;

    if (client->__cmd.size() == 2)
        for (std::vector<std::string>::iterator it = client->__cmd.begin() + 1; it != client->__cmd.end(); it++)
            reason += *it + " ";
    for (std::map<std::string, Channel*>::iterator it_channel = this->get_channels().begin(); it_channel != this->get_channels().end(); it_channel++) {
        if (it_channel->second->find_client(client->get_socket()))
            it_channel->second->delete_client(client->get_socket());
    }
    close(client->get_socket());
    this->delete_client(client->get_socket());
    system("leaks ircserv");
}

void    Mediator::command_not_found(Client *client) {
    std::string error;

    error = ":ft_irc 421 " + client->get_nickname() + " " + client->__cmd[0] + " :Unknown command.";
    client->put_message(error);
}

bool    Mediator::search_channel(std::string name, std::map<std::string, Channel*>     __channels){
    if (__channels.find(name) == __channels.end()) {
        return false;
    } else {
        return true;
    }
}

void    Mediator::topic_cmd(Client *client){
    if (client->__cmd.size() < 2) {
        client->put_message(":ft_irc 461 " + client->get_nickname() +" :Not enough parameters");
        return;
    } else {
        if (client->__cmd[1][0] != '#'){
            client->put_message(":ft_irc 476 " + client->get_nickname() +" :Bad Channel Mask");
            return ;
        }
        if (client->__cmd[1].size() == 1){
            client->put_message(":ft_irc 480 " + client->get_nickname() +" :you need name of channel");
            return ;
        }
        if (!search_channel(client->__cmd[1], this->__channels)){
            client->put_message(":ft_irc 403 " + client->get_nickname() + " :No such channel");
            return;
        } else {
            Channel *channel = NULL;
            channel = client->get_channel(client->__cmd[1]);
            if (channel == NULL) {
                client->put_message(":ft_irc 442 " + client->get_nickname() + " :You're not on that channel");
                return;
            }
            if (!channel->find_operator(client->get_socket())){
                client->put_message(":ft_irc 482 " + client->get_nickname() + " :You're not channel operator");
            }
            else {
                if (client->__cmd.size() >= 3) {
                    if (channel->get_modetopic()) {
                        if (client->__cmd[2] == ":" && channel)
                            channel->set_topic("");
                        else if (channel)
                            channel->set_topic(client->__cmd[2].substr(1));
                    } else {
                        client->put_message(":ft_irc don't have mode topic");
                        return ;
                    }
                }
            }
        }
    }
}
// @id=234AB :dan!d@localhost PRIVMSG #chan :Hey what's up!

Client* Mediator::get_client(std::string &nick_name) {
    std::map<int, Client *>::iterator it;
    for (it = this->__clients.begin(); it != this->__clients.end(); ++it) {
        if (it->second->get_nickname() == nick_name) {
            return it->second;
        }
    }
    return NULL;
}

bool Mediator::find_client(std::string &nick_name) {
    std::map<int, Client *>::iterator it;
    for (it = this->__clients.begin(); it != this->__clients.end(); ++it) {
        if (it->second->get_nickname() == nick_name) {
            return true;
        }
    }
    return false;
}

#include <sys/socket.h>
void Mediator::invite_cmd(Client *client) {
    if (client->__cmd.size() < 3) {
        std::string msg = ":ft_irc 341 " + client->get_nickname() + " :Not enough parameters\r\n";
        if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
            perror("send");
            return ;
        }
        return ;
    }
    if (this->__channels.find(client->__cmd[2]) == this->__channels.end()) {
        std::string msg = ":ft_irc 403 " + client->get_nickname() + " " + client->__cmd[2] + " :No such channel\r\n";
        if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
            perror("send");
            return ;
        }
        return ;
    }
    if (!this->find_client(client->__cmd[1])) {
        std::string msg = ":ft_irc 401 " + client->get_nickname() + " " + client->__cmd[1] + " :No such nick\r\n";
        if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
            perror("send");
            return ;
        }
        return ;
    }
    Channel *invite_channel = this->__channels.at(client->__cmd[2]);
    if (!invite_channel) {
        return ;
    }
    if (invite_channel->get_mode()) {
        if (invite_channel->find_operator(client->get_socket())) {
            if (invite_channel->is_invited(this->get_client(client->__cmd[1])->get_socket())) {
                std::string msg = ":ft_irc 555 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] + " :is already invited\r\n";
                if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                    perror("send");
                    return ;
                }
                return ;
            }
            if (invite_channel->find_client(client->__cmd[1])) {
                std::string msg = ":ft_irc 443 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] + " :is already on channel\r\n";
                if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                    perror("send");
                    return ;
                }
                return ;
            } else {
                std::string msg = ":ft_irc 341 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] + "\r\n";
                if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                    perror("send");
                    return ;
                }
                msg = ":" + client->get_nickname() + " " + "INVITE " + client->get_nickname() + " " + client->__cmd[2] + "\r\n";
                Client *invited_client = this->get_client(client->__cmd[1]);
                if (!invited_client) {
                    return ;
                }
                if (send(invited_client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                    perror("send");
                    return ;
                }
                return ;
                // invite the nick
            }
        } else {
            std::string msg = ":ft_irc 482" + client->get_nickname() + " " + client->__cmd[2] + " :You're not channel operator\r\n";
            if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                perror("send");
                return ;
            }
            return ;
        }
    } else {
        if (!invite_channel->find_client(client->get_socket())) {
            std::string msg = ":ft_irc 442 " + client->get_nickname() + " " + client->__cmd[2] + " :You're not on that channel\r\n";
            if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                perror("send");
                return ;
            }
            return;
        }
        if (invite_channel->find_client(client->__cmd[1])) {
            std::string msg = ":ft_irc 443 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] + " :is already on channel\r\n";
            if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                perror("send");
                return ;
            }
            return ;
        } else {
            if (invite_channel->is_invited(this->get_client(client->__cmd[1])->get_socket())) {
                std::string msg = ":ft_irc 555 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] + " :is already invited\r\n";
                if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                    perror("send");
                    return ;
                }
                return ;
            }
            std::string msg = ":ft_irc 341 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] + "\r\n";
            if (send(client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                perror("send");
                return ;
            }
            msg = ":" + client->get_nickname() + " " + "INVITE " + client->get_nickname() + " " + client->__cmd[2] + "\r\n";
            Client *invited_client = this->get_client(client->__cmd[1]);
            if (!invited_client) {
                return ;
            }
            if (send(invited_client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                perror("send");
                return ;
            }
            return ;
            // invite the nick
        }
    }
}
