#include "topic_cmd.class.hpp"

Topic::Topic(Client *client): __client(client) {
}

Topic::~Topic(){}

void Topic::execute(Mediator* mediator) {
        mediator->topic_cmd(__client);
    }
