#ifndef PART_CMD_CLASS_HPP
# define PART_CMD_CLASS_HPP

#include "client.class.hpp"
#include "channel.class.hpp"
#include "command.interface.hpp"
#include "mediator.class.hpp"

class Part : public Command {
    private:
        Client* __client;
        Channel* __channel;
    public:
        Part(Client *client, Channel *channel);
        virtual ~Part();

        virtual void execute(Mediator* mediator);
};



#endif











































