#include "quit_cmd.class.hpp"

Quit::Quit(Client *client): __client(client){}

Quit::~Quit(){}

void Quit::execute(Mediator* mediator){
    mediator->quit_cmd(__client);
}