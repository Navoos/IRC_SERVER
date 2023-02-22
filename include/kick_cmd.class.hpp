#ifndef KICK_CMD_CLASS_HPP
# define KICK_CMD_CLASS_HPP

#include "client.class.hpp"
#include "channel.class.hpp"
#include "command.interface.hpp"
#include "mediator.class.hpp"

class Kick : public Command {
    private:
        Client *__client;
        Channel *__channel;
    public:
        Kick(Client *client, Channel *channel);
        virtual ~Kick();

        virtual void execute(Mediator* mediator);
};


#endif