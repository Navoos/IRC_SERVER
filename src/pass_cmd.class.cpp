#include "pass_cmd.class.hpp"

Pass::Pass(Client *client): __client(client){}

Pass::~Pass(){}

void Pass::execute(Mediator* mediator){
    mediator->pass_cmd(__client);
}


