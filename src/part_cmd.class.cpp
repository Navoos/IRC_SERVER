#include "part_cmd.class.hpp"

Part::Part(Client* client, Channel* channel) : client(client), channel(channel) {}

Part::~Part(){}

void Part::execute(Mediator* mediator) {
        mediator->part(client, channel);
    }