
#include "user_cmd.class.cpp"

User::User(Client *client): __client(client){}

User::~User(){}

void User::execute(Mediator* mediator){
    mediator->user_cmd(__client);
}