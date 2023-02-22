#include "join_cmd.class.hpp"

Join::Join(Client* client, Channel* channel) : __client(client), __channel(channel) {}

Join::~Join(){}

void Join::execute(Mediator* mediator) {
        mediator->join_cmd(__client, __channel);
    }