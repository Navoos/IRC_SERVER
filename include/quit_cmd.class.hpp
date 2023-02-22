#ifndef QUIT_CMD_CLASS_HPP
# define QUIT_CMD_CLASS_HPP

#include "client.class.hpp"
#include "command.interface.hpp"
#include "mediator.class.hpp"

class Quit :public Command {
    private:
        Client *__client;
    public:
        Quit(Client *client);
        virtual ~Quit();
    
        virtual void execute(Mediator* mediator);
};

#endif