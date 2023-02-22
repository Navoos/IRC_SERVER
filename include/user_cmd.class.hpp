#ifndef USER_CMD_CLASS_HPP
# define USER_CMD_CLASS_HPP

#include "client.class.hpp"
#include "command.interface.hpp"
#include "mediator.class.hpp"

class User : public Command {

    private:
        Client *__client;
    public:
        User(Client client);
        virtual ~User();

        virtual void execute(Mediator* mediator);
};

#endif