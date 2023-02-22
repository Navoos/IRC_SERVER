#ifndef CLIENT_CLASS_HPP
# define CLIENT_CLASS_HPP
#include <vector>
#include <string>

class Mediator;

class Client {
  public:
    /* EXAMPLE:
     * void join(Channel* channel) {
        Command* command = new Join(this, channel);
        command->execute(mediator);
        delete command;
    }*/
    // HOUSSAM add additional commands here
  private:
    const std::string   __buffer;
    std::vector<Channels*> channels;
    Mediator *__mediator;
    friend class Mediator;
};
#endif
