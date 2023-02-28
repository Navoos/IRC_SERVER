#ifndef MODE_CMD_CLASS_HPP
# define MODE_CMD_CLASS_HPP

#include "client.class.hpp"
#include "channel.class.hpp"
#include "command.interface.hpp"
#include "mediator.class.hpp"

class Mode :public Command {
    private:
        Client *__client;
        Channel *__channel;
    public:
        Mode(Client *client, Channel *channel);
        virtual ~Mode();
    
        virtual void execute(Mediator* mediator);
};

#endif