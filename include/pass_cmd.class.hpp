
#ifndef PASS_CMD_CLASS_HPP
# define PASS_CMD_CLASS_HPP

#include "command.interface.hpp"

class Client;
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