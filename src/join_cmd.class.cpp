#include "join_cmd.class.hpp"

Join::Join(Client* client) : __client(client) {}

Join::~Join(){}

void Join::execute(Mediator* mediator) {
        mediator->join_cmd(__client);
    }
