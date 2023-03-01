#include "mediator.class.hpp"
#include "client.class.hpp"
#include "server.class.hpp"
#include <iostream>

static bool valid_name(std::string nickname){
    if (nickname.size() > 9)
        return false;
    for (size_t i = 0; i < nickname.size(); i++){
        if (!std::isalpha(nickname[i]) && !std::isdigit(nickname[i]))
        return false;
    }
    return true;
}
void Mediator::pass_cmd(Client *client, Server *server) {
    if (client->is_connected()) {
        client->put_message(ERR_ALREADYREGISTRED, "registred");
        return;
    }
    if (client->__cmd.size() != 2) {
        
        client->put_message(ERR_NEEDMOREPARAMS, "No parameters");
        return;
    }
    if (client->__cmd[1] == server->get_password()) {
        client->set_accepted(true);
        return;
    }
    else {
        client->put_message(ERR_PASSWDMISMATCH, "Password incorrect");
        return; 
    }
    
    client->check_connection();
    return;
}

void Mediator::user_cmd(Client *client){
    if (client->is_connected()){
        client->put_message(ERR_ALREADYREGISTRED, "registred");
    }
    if (client->__cmd.size() < 5){
		client->put_message(ERR_NEEDMOREPARAMS, "No parameters");
        return ;
    }
    client->check_connection();
    return;
}

void Mediator::nick_cmd(Client *client){
    if (client->is_connected()){
        std::cout << "Your connection is restricted!";
        return;
    }

	if (client->__cmd.size() != 2 || client->__cmd[1].length() == 0)
    {
		client->put_message(ERR_NONICKNAMEGIVEN, "Not given nickname");
        return ;
    }

    if (!valid_name(client->__cmd[1])){
        client->put_message(ERR_ERRONEUSNICKNAME, "Error nickname");
        return ;
    } else {
        
        for (std::map<int,Client>::iterator it = __server->get_clients().begin(); it != __server->get_clients().end();++it) {
            if (it->second.get_nickname() == client->__cmd[1]) {
                client->put_message(ERR_NICKNAMEINUSE, ":Nickname is already in use");
                return ;
            }
        }
    }
    client->check_connection();
    return;
}