
#ifndef PASS_CMD_CLASS_HPP
# define PASS_CMD_CLASS_HPP

#include "client.class.hpp"
#include "command.interface.hpp"
#include "mediator.class.hpp"

class Pass : public Command
{
private:
    Client *__client;
public:
    Pass(Client *client);
    virtual ~Pass();
    
    virtual void execute(Mediator* mediator);
};

#endif