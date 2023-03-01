
#ifndef PASS_CMD_CLASS_HPP
# define PASS_CMD_CLASS_HPP

#include "client.class.hpp"
#include "command.interface.hpp"
#include "mediator.class.hpp"

class Server;

class Pass : public Command
{
private:
    Client *__client;
    Server *__server;

public:
    Pass(Client *client, Server *server);
    virtual ~Pass();
    
    virtual void execute(Mediator* mediator);
};

#endif