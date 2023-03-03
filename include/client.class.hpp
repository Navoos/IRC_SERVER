#ifndef CLIENT_CLASS_HPP
# define CLIENT_CLASS_HPP
#include <vector>
#include <string>
#include "channel.class.hpp"

class Mediator;

class Client {
  public:
    bool    is_connected();
    void    put_message(std::string name, std::string message);
    Client(int fd);
    void    update_client(std::string &str);
  private:
    std::vector<std::string> __cmd;
	std::string   __buffer;
	int           __fd;
    Mediator *__mediator;
    friend class Mediator;

    bool    __connecte;


	/* EXAMPLE:
	 * void join(Channel* channel) {
		Command* command = new Join(this, channel);
		command->execute(mediator);
		delete command;
	}*/
	// HOUSSAM add additional commands here
};
#endif
