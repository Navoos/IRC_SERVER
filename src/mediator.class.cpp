#include "mediator.class.hpp"
#include "client.class.hpp"
#include "channel.class.hpp"

#include <iostream>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <string>

Mediator::Mediator(Server& server) : __server(server) {
    this->__quitter_fd = -1;
}

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
        return;
    }

    if (client->__cmd.size() != 2) {
        client->put_message(":ft_irc 461 " + client->get_nickname() +" :Not enough parameters");
        return;
    }
    if (client->__cmd[1] == server.get_password()) {
        client->set_accepted(true);
    }
    else {
        client->put_message(":ft_irc 464 " + client->get_nickname() +" :Password incorrect");
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
        return;
    }

	if (client->__cmd.size() != 2 || client->__cmd[1].length() == 0)
    {
        client->put_message(":ft_irc 431 " + client->get_nickname() +" :Not given nickname");
        return;
    }

    if (!valid_name(client->__cmd[1])){
        client->put_message(":ft_irc 432 " + client->get_nickname() +" :Erroneus nickname");
        return;
    } else {
        
        for (std::map<int,Client *>::iterator it = this->__clients.begin(); it != this->__clients.end(); ++it) {
            if (it->second->get_nickname() == client->__cmd[1]) {
                client->put_message(":ft_irc 433 " + client->get_nickname() +" :Nickname is already in use");
                return;
            }
        }
    }
    client->set_nickname(client->__cmd[1]);
    client->check_connection();
    return;
}

void    Mediator::delete_client(int fd) {
    if (this->__clients.find(fd) == this->__clients.end())
        return ;
    Client *client  = this->__clients.at(fd);
    std::string msg = ":" + (!client->get_nickname().empty() ? client->get_nickname() : "no_nick") + "!" + (!client->get_username().empty() ? client->get_username() : "no_user") + "@" + client->get_hostname() + " QUIT :Quit: " + this->__quitter_reason + "\r\n";
    for (std::map<std::string, Channel *>::iterator it = client->get_all_channels().begin(); it != client->get_all_channels().end();++it) {
        for (std::map<int, Client*>::iterator itt = it->second->get_all_client().begin(); itt != it->second->get_all_client().end(); ++itt) {
            if (send(itt->second->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                perror("send");
            }
        }
    }
    for (std::map<std::string, Channel *>::iterator it = this->__channels.begin(); it != this->__channels.end();++it) {
        if (it->second->find_client(fd))
            it->second->delete_client(fd);
        if (it->second->find_operator(fd))
            it->second->delete_moderator(fd);
        if (it->second->find_invited(fd))
            it->second->delete_invited(fd);
    }
    this->__clients.erase(fd);
    delete client;
}

void    Mediator::set_quit(int fd, std::string reason) {
    this->__quitter_fd = fd;
    this->__quitter_reason = reason;
}

void  Mediator::set_quit_reason(std::string reason) {
    this->__quitter_reason = reason;
}

int    Mediator::get_quit() {
    return this->__quitter_fd;
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
    Client *client = NULL;
    if (this->__clients.find(fd) != this->__clients.end()) {
        client = this->__clients.at(fd);
    } else {
        client = new Client(fd, password, mediator);
        if (!client)
            return ;
        this->__clients.insert(std::make_pair(fd, client));
    }
    if (!client)
        return ;
    client->update_client(buffer);
}

void  Mediator::add_client(int fd, std::string &password, Mediator *mediator, struct sockaddr &addr) {
    Client *client = NULL;
    client = new Client(fd, password, mediator, addr);
    if (!client)
        return ;
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


void Mediator::notify_clients_of_new_member(Channel *channel, Client *client) {
    std::map<int, Client*> all_clients = channel->get_all_client();
    for (std::map<int, Client*>::iterator it = all_clients.begin(); it != all_clients.end(); ++it) {
        std::string msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " JOIN " + channel->get_name() + "\r\n";
        if (send(it->second->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
            perror("send");
        }
   }
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
            notify_clients_of_new_member(channel, client);
            // client->put_message(":" + client->get_nickname() + " JOIN " + channel->get_name());
        } else {
            Channel *channel = this->__channels.at(*it);
            if (channel && channel->get_all_client().size() == 0) { // yaakoub add this lines
                channel->add_moderator(client->get_socket());
            }
            if (channel && channel->find_client(client->get_socket())) {
                client->put_message(":ft_irc 480 " + client->get_nickname() +" :is already on channel");
                return ;
            }
            if (channel && channel->get_modeinvite()) {
                if (channel->is_invited(client->get_socket())) {
                    if (!keys.empty() && j < (int)keys.size()) {
                        if (keys[j] == channel->get_key()) {
                                channel->add_client(client);
                                client->subscribe_to_channel(channel);
                                notify_clients_of_new_member(channel, client);
                                // client->put_message(":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " JOIN " + channel->get_name());
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
                                notify_clients_of_new_member(channel, client);
                                // client->put_message(":" + client->get_nickname() + " JOIN " + channel->get_name());
                        } else {
                            client->put_message(":ft_irc 475 " + client->get_nickname() + " " + channel->get_name() + " :Cannot join channel (+k)");
                        }
                        } else {
                            channel->add_client(client);
                            client->subscribe_to_channel(channel);
                            notify_clients_of_new_member(channel, client);
                            // client->put_message(":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " JOIN " + channel->get_name());
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
        error = ":ft_irc 461 " + client->get_nickname() + " :Not enough parameters.";
        client->put_message(error);
        return ;
    } else {
        __channels = split(client->__cmd[1], ',');
        if (__channels.size() == 0) {
            error = ":ft_irc 911 " + client->get_nickname() + " :you should write at least one channel.";
            client->put_message(error);
            return ;
        }
        int i = 0;
        for (std::vector<std::string>::iterator it_reason = client->__cmd.begin() + 2; it_reason != client->__cmd.end(); ++it_reason)
            reason += *it_reason + " ";
        for (std::vector<std::string>::iterator it = __channels.begin(); it != __channels.end(); ++it, ++i) {
            if (__channels[i][0] != '#') {
                error = ":ft_irc 476 " + client->get_nickname() + " :Bad channel mask.";
                client->put_message(error);
                continue ;
            } else {
                if (__channels[i][1] == '\0') {
                    error = ":ft_irc 913 " + client->get_nickname() + " :you need name of channel.";
                    client->put_message(error);
                    continue ;
                }
                if (this->__channels.find(*it) == this->__channels.end()) {
                    error = ":ft_irc 403 " + client->get_nickname() + " :No such channel.";
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
                            message = ":ft_irc " + it1->second->get_nickname() + " :" + client->get_nickname() + " is leaving the channel " + __channels[i] + " because \"" + reason + "\".";
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
                    error = ":ft_irc 441 " + client->get_nickname() + " :You're not on that channel.";
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

    if (client->__cmd.size() < 3){
        error = ":ft_irc 461 " + client->get_nickname() + "  :Not enough parameters.";
        client->put_message(error);
        return ;
    } else {
        int         i = 0;
        __channels = split(client->__cmd[1], ',');
        clients = split(client->__cmd[2], ',');
        if (__channels.size() == 0) {
            error = ":ft_irc 911 " + client->get_nickname() + "  :you should write at least one channel.";
            client->put_message(error);
            return ;
        }
        if (clients.size() == 0) {
            error = ":ft_irc 912 " + client->get_nickname() + "  :you should write at least one user.";
            client->put_message(error);
            return ;
        }
        for (unsigned int i = 3; i < client->__cmd.size(); ++i) {
            comment += client->__cmd[i];
            if (i < client->__cmd.size() - 1)
                comment += " ";

        }
        for (std::vector<std::string>::iterator it = __channels.begin(); it != __channels.end(); ++it, ++i) {
            Channel *channel = client->get_channel(*it);
            if (__channels[i][0] != '#') {
                error = ":ft_irc 476 " + *it + " :Bad channel mask";
                client->put_message(error);
                continue ;
            } else {
                if (__channels[i][1] == '\0') {
                    error = ":ft_irc 913 " + client->get_nickname() + " :You need name of channel.";
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
                                            message = ":" + client->get_nickname() + " KICK " + __channels[i] + " " + *it_clients;
                                        } else {
                                            message = ":" + client->get_nickname() + " KICK " + __channels[i] + " " + *it_clients + " " + comment;
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
                                                message = ":ft_irc " + it_client->second->get_nickname() + " is the new operator of channel " + __channels[i] + ".";
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
                            message = ":ft_irc " + client->get_nickname() + " setting the invited mode on " + channel->get_name() + ".";
                            client->put_message(message);
                        } else if (modestring[1] == 't') {
                            channel->set_modetopic(true);
                            message = ":ft_irc " + client->get_nickname() + " setting the topic mode on " + channel->get_name() + ".";
                            client->put_message(message);
                        } else if (modestring[1] == 'k') {
                            if (client->__cmd.size() == 4) {
                                channel->set_modekey(true);
                                channel->set_key(key);
                                message = ":ft_irc " + channel->get_name() + " :Set the channel key to " + key + ".";
                                client->put_message(message);
                                return ;
                            } else {
                                error = ":ft_irc 472 " + client->get_nickname() + " :Is unknown mode char to me.";
                                client->put_message(error);
                                return ;
                            }
                        } else if (modestring[1] == 'o') {
                            if (!user.empty()) {
                                if (channel->find_client(user)) {
                                    if (!channel->find_operator(channel->get_client(user))) {
                                        channel->add_moderator(channel->get_client(user));
                                        for (std::map<int, Client*>::iterator it_client = channel->get_all_client().begin(); it_client != channel->get_all_client().end(); it_client++) {
                                            message = ":ft_irc " + client->get_nickname() + " :Set the operator to " + user + ".";
                                            it_client->second->put_message(message);
                                        }
                                    } else {
                                        error = ":ft_irc 914 " + target + " :Is a operator.";
                                        client->put_message(error);
                                        return ;
                                    }
                                } else {
                                    error = ":ft_irc 401 " + client->get_nickname() + " " + target + " :No such nick/channel.";
                                    client->put_message(error);
                                    return ;
                                }
                            } else {
                                error = ":ft_irc 916 :You should write a client.";
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
                                            error = ":ft_irc 915 " + user + " :Is not a operator.";
                                            client->put_message(error);
                                            return ;
                                        }
                                    } else {
                                        error = ":ft_irc 401 " + client->get_nickname() + " " + user + " :No such nick/channel.";
                                        client->put_message(error);
                                        return ;
                                    }
                                } else {
                                    error = ":ft_irc 916 :You should write a client.";
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
        error = ":ft_irc 476 " + target + " :Bad channel mask";
        client->put_message(error);
        return ;
    }
    //     std::cout << "dont enter here" << std::endl;
    //     if (modestring[1] == 'o') {
    //         for (std::map<std::string, Channel*>::iterator it = this->get_channels().begin(); it != this->get_channels().end(); it++) {
    //             std::cout << "enter here" << std::endl;
    //             if (it->second->find_client(client->get_socket())) {
    //                 if (it->second->find_operator(client->get_socket())) {
    //                     if (it->second->find_client(target)) {
    //                         if (modestring == "+o") {
    //                             if (!it->second->find_operator(it->second->get_client(target))) {
    //                                 it->second->add_moderator(it->second->get_client(target));
    //                                 for (std::map<int, Client*>::iterator it_client = it->second->get_all_client().begin(); it_client != it->second->get_all_client().end(); it_client++) {
    //                                     message = ":ft_irc " + client->get_nickname() + " +o :Set the operator to " + target + ".";
    //                                     it_client->second->put_message(message);
    //                                 }
    //                             } else {
    //                                 error = ":ft_irc 914 " + target + " :Is a operator.";
    //                                 client->put_message(error);
    //                                 return ;
    //                             }
    //                         } else if (modestring == "-o") {
    //                             if (it->second->find_operator(it->second->get_client(target))) {
    //                                 it->second->delete_moderator(it->second->get_client(target));
    //                                 for (std::map<int, Client*>::iterator it_client = it->second->get_all_client().begin(); it_client != it->second->get_all_client().end(); it_client++) {
    //                                     message = ":ft_irc " + client->get_nickname() + " -o :Remove the operator from " + target + ".";
    //                                     it_client->second->put_message(message);
    //                                 }
    //                             } else {
    //                                 error = ":ft_irc 915 " + target + " :Is not a operator.";
    //                                 client->put_message(error);
    //                                 return ;
    //                             }
    //                         } else {
    //                             error = ":ft_irc 501 " + client->get_nickname() + " :Unknown MODE flag.";
    //                             client->put_message(error);
    //                             return ;
    //                         }
    //                     } else {
    //                         error = ":ft_irc 401 " + client->get_nickname() + " " + target + " :No such nick/channel.";
    //                         client->put_message(error);
    //                         return ;
    //                     }
    //                 } else {
    //                     error = ":ft_irc 482 " + client->get_nickname() + " " + it->second->get_name() + " :You're not channel operator.";
    //                     client->put_message(error);
    //                     return ;
    //                 }
    //             } else {
    //                 error = ":ft_irc 442 " + client->get_nickname() + " " + it->second->get_name() + " :You're not on that channel.";
    //                 client->put_message(error);
    //                 return ;
    //             }
    //         }
    //     } else {
    //         error = ":ft_irc 501 " + client->get_nickname() + " :Unknown MODE flag.";
    //         client->put_message(error);
    //         return ;
    //     }
    // }
}

// TODO: yaakoub dir shi tawil m3a had lqlawi dial quit
void    Mediator::quit_cmd(Client *client) 
{
    std::string reason = "";
    //
    // if (client->__cmd.size() == 2)
    //     for (std::vector<std::string>::iterator it = client->__cmd.begin() + 1; it != client->__cmd.end(); it++)
    //         reason += *it + " ";
    // for (std::map<std::string, Channel*>::iterator it_channel = this->get_channels().begin(); it_channel != this->get_channels().end(); it_channel++) {
    //     if (it_channel->second->find_client(client->get_socket()))
    //         it_channel->second->delete_client(client->get_socket());
    // }
    // close(client->get_socket());
    // this->delete_client(client->get_socket());
    if (client->__cmd.size() >= 2) {
        for (unsigned int i = 2; i < client->__cmd.size();++i) {
            reason += client->__cmd[i];
            if (i < client->__cmd.size() - 1)
                reason += " ";
        }
    }
    for (std::map<std::string, Channel*>::iterator it_channel = this->get_channels().begin(); it_channel != this->get_channels().end(); it_channel++) {
        if (it_channel->second->find_client(client->get_socket()))
            it_channel->second->delete_client(client->get_socket());
    }
    this->set_quit(client->get_socket(), reason);
}

void    Mediator::time_cmd(Client *client) {
    time_t current_time;
    std::string curr_time;
    std::vector<std::string> first_time;
    std::string date;

    time(&current_time);
    curr_time = ctime(&current_time);
    first_time = split(curr_time, ' ');
    date = RED + first_time[0] + " " + first_time[2] + " " + GREEN + first_time[1] + " " + BLUE + first_time[4] + RESET;
    date.erase(date.size() - 5, date.size() - 1);
    client->put_message(date);
    std::string time = YELLOW + first_time[3] + RESET;
    time = "    " + time;
    client->put_message(time);
}

void    Mediator::find_cmd(Client *client) {
    std::string message;
    if (client->__cmd.size() < 2) {
        std::string error = ":ft_irc 461 " + client->get_nickname() + " :Not enough parameters.";
        client->put_message(error);
        return ;
    }
    for (std::map<int, Client*>::iterator it = this->__clients.begin(); it != this->__clients.end(); it++) {
        if (it->second->get_nickname() == client->__cmd[1]) {
            message = ":ft_irc " + client->__cmd[1] + " :This nickname is used before.";
            client->put_message(message);
            return ;
        } else {
            message = ":ft_irc " + client->__cmd[1] + " :This nickname still free.";
            client->put_message(message);
            return ;
        }
    }
}

void    Mediator::commands(Client *client) {
    client->put_message("PASS : The password of server");
    client->put_message("USER : The username of client that you want");
    client->put_message("NICK : The nickname of client that you want");
    client->put_message("JOIN : For join a client to a channel");
    client->put_message("TOPIC : Set a title to a channel");
    client->put_message("PART : If a client need to leave a channel he can use this command");
    client->put_message("KICK : For kick a client from a channel");
    client->put_message("INVITE : To invite a client to a channel");
    client->put_message("PRIVMSG : To sent a private messge to another client");
    client->put_message("MODE : To set option to a channel or user");
    client->put_message("QUIT : If a client need to leave the server he can use this command");
    client->put_message("NOTICE : To sent a private messsage but without errors");
    client->put_message("           _           _   ");
    client->put_message("          | |         | |  ");
    client->put_message(" _ __ ___ | |__   ___ | |_ ");
    client->put_message("| '__/ _ \\| '_ \\ / _ \\| __|");
    client->put_message("| | | (_) | |_) | (_) | |_ ");
    client->put_message("|_|  \\___/|_.__/ \\___/ \\__|");
    client->put_message("/COMMANDS : For display all commands");
    client->put_message("/FIND : For check a nickname if used before or not");
    client->put_message("/JOKE : For tell you some jokes");
    client->put_message("/TIME : For display the current time");
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
                client->put_message(":ft_irc 482 " + client->get_nickname() + " " + channel->get_name() + " :You're not channel operator");
                return ;
            }
            else {
                std::string topic = "";
                if (client->__cmd.size() >= 3) {
                    if (channel->get_modetopic()) {
                        if (client->__cmd[2] == ":" && channel)
                            channel->set_topic(topic);
                        else if (channel) {
                            if (client->__cmd[2][0] == ':') {
                                topic += client->__cmd[2].substr(1);
                                for (unsigned int i = 3;i < client->__cmd.size();++i) {
                                    topic += client->__cmd[i];
                                    if (i < client->__cmd.size() - 1)
                                        topic += " ";
                                }
                            }
                            else
                                topic = client->__cmd[2];
                        }
                        client->put_message(":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " " + "TOPIC " + channel->get_name() + " " + topic);
                    } else {
                       client->put_message(":" + client->get_nickname() + " " + "TOPIC " + channel->get_name() + " :don't have privileges");
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
// client->put_message(":ft_irc 461 " + client->get_nickname() +" :Not enough parameters");

void Mediator::invite_cmd(Client *client) {
    if (client->__cmd.size() < 3) {
        std::string msg = ":ft_irc 461 " + client->get_nickname() + " :Not enough parameters";
        client->put_message(msg);
        return ;
    }
    if (this->__channels.find(client->__cmd[2]) == this->__channels.end()) {
        std::string msg = ":ft_irc 403 " + client->get_nickname() + " " + client->__cmd[2] + " :No such channel";
        client->put_message(msg);
        return ;
    }
    if (!this->find_client(client->__cmd[1])) {
        std::string msg = ":ft_irc 401 " + client->get_nickname() + " " + client->__cmd[1] + " :No such nick";
        client->put_message(msg);
        return ;
    }
    Channel *invite_channel = this->__channels.at(client->__cmd[2]);
    if (!invite_channel) {
        return ;
    }
    if (invite_channel->get_modeinvite()) {
        if (invite_channel->find_operator(client->get_socket())) {
            if (invite_channel->is_invited(this->get_client(client->__cmd[1])->get_socket())) {
                std::string msg = ":ft_irc 555 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] + " :is already invited";
                client->put_message(msg);
                return ;
            }
            if (invite_channel->find_client(client->__cmd[1])) {
                std::string msg = ":ft_irc 443 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] + " :is already on channel";
                client->put_message(msg);
                return ;
            } else {
                std::string msg = ":ft_irc 341 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2];
                client->put_message(msg);
                msg = ":" + client->get_nickname() + " " + "INVITE " + client->get_nickname() + " " + client->__cmd[2];
                Client *invited_client = this->get_client(client->__cmd[1]);
                if (!invited_client) {
                    return ;
                }
                if (send(invited_client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                    perror("send");
                    return ;
                }
                return ;
            }
        } else {
            std::string msg = ":ft_irc 482" + client->get_nickname() + " " + client->__cmd[2] + " :You're not channel operator";
            client->put_message(msg);
            return ;
        }
    } else {
        if (!invite_channel->find_client(client->get_socket())) {
            std::string msg = ":ft_irc 442 " + client->get_nickname() + " " + client->__cmd[2] + " :You're not on that channel";
            client->put_message(msg);
            return;
        }
        if (invite_channel->find_client(client->__cmd[1])) {
            std::string msg = ":ft_irc 443 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] + " :is already on channel";
            client->put_message(msg);
            return ;
        } else {
            if (invite_channel->is_invited(this->get_client(client->__cmd[1])->get_socket())) {
                std::string msg = ":ft_irc 555 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] + " :is already invited";
                client->put_message(msg);
                return ;
            }
            std::string msg = ":ft_irc 341 " + client->get_nickname() + " " + client->__cmd[1] + " " + client->__cmd[2] ;
            client->put_message(msg);
            msg = ":" + client->get_nickname() + " " + "INVITE " + client->get_nickname() + " " + client->__cmd[2] ;
            Client *invited_client = this->get_client(client->__cmd[1]);
            if (!invited_client) {
                return ;
            }
            if (send(invited_client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                perror("send");
                return ;
            }
            return ;
        }
    }
}

void Mediator::privmsg_cmd(Client *client) {
    if (client->__cmd.size() < 3) {
        std::string msg = ":ft_irc 461 " + client->get_nickname() + " PRIVMSG " + " :Not enough parameters";
        client->put_message(msg);
        return ;
    }
    std::vector<std::string> targets = split(client->__cmd[1], ',');
    std::string message_to_be_sent = "";
    message_to_be_sent += client->__cmd[2];
    if (client->__cmd.size() > 3)
        message_to_be_sent += " ";
    for (unsigned int i = 3; i < client->__cmd.size();++i) {
        message_to_be_sent += client->__cmd[i];
        if (i < client->__cmd.size() - 1)
            message_to_be_sent += " ";
    }
    for (unsigned int i = 0;i < targets.size();++i) {
        if (targets[i][0] == '#') {
            if (this->__channels.find(targets[i]) == this->__channels.end()) {
                std::string msg = ":ft_irc 403 " + client->get_nickname() + " " + client->__cmd[1] + " :No such channel";
                client->put_message(msg);
                continue ;
            } else {
                // if channel is moderated the only who has the voice command and operator can send messages
                Channel *channel = this->__channels.at(targets[i]);
                if (!channel)
                    continue;
                if (!channel->is_moderated()) {
                    // send normally
                    std::string msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " PRIVMSG " + channel->get_name() + " " + message_to_be_sent + "\r\n";
                    for (std::map<int, Client *>::iterator it = channel->get_all_client().begin(); it != channel->get_all_client().end();++it) {
                        if (send(it->first, msg.c_str(), msg.length(), 0) == -1)
                        {
                            perror("send");
                            continue;
                        }
                    }
                    continue;
                }
                if (channel->is_moderated() && (channel->find_operator(client->get_socket()) || client->has_voice())) {
                    // send message to all clients currently connected
                    for (std::map<int, Client *>::iterator it = channel->get_all_client().begin(); it != channel->get_all_client().end();++it) {
                        if (send(it->first, client->__cmd[2].c_str(), client->__cmd[2].length(), 0) == -1)
                        {
                            perror("send");
                            continue;
                        }
                    }
                    continue;
                } else {
                    std::string msg = ":ft_irc 404 " + client->get_nickname() + " " + targets[i] + " :Cannot send to channel";
                    client->put_message(msg);
                    continue ;
                }
            }
       } else {
           Client *send_client = get_client(targets[i]);
           if (send_client) {
                std::string msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " PRIVMSG " + send_client->get_nickname() + " " + message_to_be_sent + "\r\n";
                if (send(send_client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                    perror("send");
                    continue;
                }
                continue;
           } else {
                std::string msg = ":ft_irc 401 " + client->get_nickname() + " " + targets[i] + " :No such nick";
                client->put_message(msg);
                continue ;
           }
       }
    }
}
void Mediator::notice_cmd(Client *client) {
    if (client->__cmd.size() < 3) {
        return ;
    }
    std::vector<std::string> targets = split(client->__cmd[1], ',');
    std::string message_to_be_sent = "";
    message_to_be_sent += client->__cmd[2][0] == ':' ? client->__cmd[2].substr(1) : client->__cmd[2];
    if (client->__cmd.size() > 3)
        message_to_be_sent += " ";
    for (unsigned int i = 3; i < client->__cmd.size();++i) {
        message_to_be_sent += client->__cmd[i];
        if (i < client->__cmd.size() - 1)
            message_to_be_sent += " ";
    }
    for (unsigned int i = 0;i < targets.size();++i) {
        if (targets[i][0] == '#') {
            if (this->__channels.find(targets[i]) == this->__channels.end()) {
                continue ;
            } else {
                // if channel is moderated the only who has the voice command and operator can send messages
                Channel *channel = this->__channels.at(targets[i]);
                if (!channel)
                    continue;
                if (!channel->is_moderated()) {
                    // send normally
                    std::string msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " NOTICE " + channel->get_name() + " " + message_to_be_sent + "\r\n";
                    for (std::map<int, Client *>::iterator it = channel->get_all_client().begin(); it != channel->get_all_client().end();++it) {
                        if (send(it->first, msg.c_str(), msg.length(), 0) == -1)
                        {
                            perror("send");
                            continue;
                        }
                    }
                    continue;
                }
                if (channel->is_moderated() && (channel->find_operator(client->get_socket()) || client->has_voice())) {
                    // send message to all clients currently connected
                    for (std::map<int, Client *>::iterator it = channel->get_all_client().begin(); it != channel->get_all_client().end();++it) {
                        if (send(it->first, client->__cmd[2].c_str(), client->__cmd[2].length(), 0) == -1)
                        {
                            perror("send");
                            continue;
                        }
                    }
                    continue;
                } 
            }
       } else {
           Client *send_client = get_client(targets[i]);
           if (send_client) {
                std::string msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " NOTICE " + send_client->get_nickname() + " " + message_to_be_sent + "\r\n";
                if (send(send_client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                    perror("send");
                    continue;
                }
                continue;
           } 
       }
    }
}

std::string Mediator::get_random_joke() {
    std::vector<std::string> jokes;
    jokes.push_back("Why don't scientists trust atoms? Because they make up everything.");
    jokes.push_back("I told my wife she was drawing her eyebrows too high. She looked surprised.");
    jokes.push_back("Why did the tomato turn red? Because it saw the salad dressing.");
    jokes.push_back("Why did the coffee file a police report? It got mugged.");
    jokes.push_back("I'm reading a book on anti-gravity. It's impossible to put down.");
    jokes.push_back("What do you call an alligator in a vest? An investigator.");
    jokes.push_back("what do you call a cut dor... adorable.");
    jokes.push_back("what do you call someone without nose without body... no body knows.");
    int index = rand() % jokes.size();
    return jokes[index];
}

void Mediator::command_bot(Client *client) {
            std::string joke = get_random_joke();
            client->put_message("hello " + client->get_nickname() + " Joke of the Day : " + joke);
        
}
