#include "topic_cmd.class.hpp"

Topic::Topic(Client *client,Channel *channel): __client(client), __channel(channel){}

Topic::~Topic(){}

void Topic::execute(Mediator* mediator) {
        mediator->topic_cmd(__client, __channel);
    }
