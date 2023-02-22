#ifndef NICK_CMD_CLASS_HPP
# define NICK_CMD_CLASS_HPP

#include "client.class.hpp"
#include "command.interface.hpp"
#include "mediator.class.hpp"

class Nick :public Command {
    private:
        Client *__client;
    public:
        Nick(Client *client);
        virtual ~Nick();
    
        virtual void execute(Mediator* mediator);
};

#endif