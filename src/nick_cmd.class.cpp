#include "nick_cmd.class.hpp"
#include "mediator.class.hpp"

Nick::Nick(Client *client): __client(client){}

Nick::~Nick(){}

void Nick::execute(Mediator* mediator){
    mediator->nick_cmd(__client);
}