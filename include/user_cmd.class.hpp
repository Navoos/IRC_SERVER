#ifndef USER_CMD_CLASS_HPP
# define USER_CMD_CLASS_HPP
#include "mediator.class.hpp"
#include "command.interface.hpp"
class Client;

class User : public Command {

    private:
        Client *__client;
    public:
        User(Client *client);
        virtual ~User();

        virtual void execute(Mediator* mediator);
};

#endif