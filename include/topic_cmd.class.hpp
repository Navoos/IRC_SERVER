#ifndef TOPIC_CMD_CLASS_HPP
# define TOPIC_CMD_CLASS_HPP
#include "mediator.class.hpp"
#include "command.interface.hpp"
class Client;

class Topic : public Command {

    private:
        Client *__client;
    public:
        Topic(Client *client);
        virtual ~Topic();

        virtual void execute(Mediator* mediator);
};

#endif