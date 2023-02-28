#ifndef CLIENT_CLASS_HPP
# define CLIENT_CLASS_HPP
#include <vector>
#include <string>
#include "channel.class.hpp"

class Mediator;

class Client {
	private:
		std::vector<std::string> __cmd;
		std::string   __buffer;
		int           __fd;
		// std::vector<Channels*> channels;
		Mediator *__mediator;
		friend class Mediator;
	public:
	Client(int fd);
    void    update_client(std::string &str);

	/* EXAMPLE:
	 * void join(Channel* channel) {
		Command* command = new Join(this, channel);
		command->execute(mediator);
		delete command;
	}*/
	// HOUSSAM add additional commands here
};
#endif
