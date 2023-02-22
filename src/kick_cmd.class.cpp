#include "kick_cmd.class.hpp"

KICK::KICK(Client* client, Channel* channel) : __client(client), __channel(channel) {}

KICK::~KICK(){}

void KICK::execute(Mediator* mediator) {
        mediator->kick_cmd(__client, __channel);
    }