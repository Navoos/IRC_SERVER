#include "kick_cmd.class.hpp"

Kick::Kick(Client* client, Channel* channel) : __client(client), __channel(channel) {}
Kick::~Kick(){}

void Kick::execute(Mediator* mediator) {
        mediator->kick_cmd(__client, __channel);
    }