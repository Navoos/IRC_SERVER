#include "mediator.class.hpp"
#include "client.class.hpp"

#include <iostream>

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
