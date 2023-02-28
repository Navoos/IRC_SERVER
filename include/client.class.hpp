#ifndef CLIENT_CLASS_HPP
# define CLIENT_CLASS_HPP
#include <vector>
#include <string>

class Mediator;

class Client {
  public:
    bool    is_connected();
    void    put_message(std::string name, std::string message);
  private:
    const std::string   __buffer;
    std::string         cmd;
    std::vector<std::string> args;
    std::vector<Channels*> channels;
    Mediator *__mediator;
    friend class Mediator;

    bool    __connecte;
};
#endif
