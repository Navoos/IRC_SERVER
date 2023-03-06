#include "part_cmd.class.hpp"

Part::Part(Client* client, Channel* channel) : __client(client), __channel(channel) {}

Part::~Part(){}

void Part::execute(Mediator* mediator) {
        mediator->part_cmd(__client, __channel);
    }