#include "pass_cmd.class.hpp"
#include "mediator.class.hpp"

Pass::Pass(Client *client, Server *server): __client(client), __server(server){}

Pass::~Pass(){}

void Pass::execute(Mediator* mediator){
    mediator->pass_cmd(__client, __server);
}


