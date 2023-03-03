#include "mode_cmd.class.hpp"

Mode::Mode(Client* client, Channel* channel) : client(client), channel(channel) {}

Mode::~Mode(){}

void Mode::execute(Mediator* mediator) {
        mediator->mode_cmd(client, channel);
    }