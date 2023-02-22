#ifndef JOIN_CMD_CLASS_HPP
# define JOIN_CMD_CLASS_HPP

#include "client.class.hpp"
#include "channel.class.hpp"
#include "command.interface.hpp"
#include "mediator.class.hpp"

class Join : public Command{

    private:
        Client *__client;
        Channel *__channel;
    public:
        Join(Client *client, Channel *channel);
        virtual ~Join();

        virtual void execute(Mediator* mediator);
};




#endif