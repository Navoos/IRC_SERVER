#include "mediator.class.hpp"
#include "client.class.hpp"

void Mediator::pass_cmd(Client *client) {
    if (client->is_connected()) {
        client->put_message(ERR_ALREADYREGISTRED, ":Unauthorized command (already registered)");
        return;
    }
    if (client->args.size() == 2) {
        
        client->put_message(ERR_NEEDMOREPARAMS, ":Not enough parameters");
        return;
    }
    if (client->args[1] == _server->getPassword()) {
        client->__connecte = true;
    } else {
        client->put_message(ERR_PASSWDMISMATCH, ":Password incorrect");
        return;
    }
    bool connected = client->check_connect();
    if (!connected) {
        std::cout << ":Can't connect to server"<< std::endl;
        return;
    }
    std::cout <<":Welcome to the server"<< std::endl;
    return;
}
