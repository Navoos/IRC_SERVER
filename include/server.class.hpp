#ifndef SERVER_CLASS_HPP
# define SERVER_CLASS_HPP

#include "socket.class.hpp"
#include "client.class.hpp"
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <utility>
#include <fcntl.h>
#include <map>
#include <unordered_map>
#define BACKLOG 10

class Client;

class Server {
  private:
	std::vector<pollfd> __fds;
	int                 __socket;
	std::map<int, Client> __clients;
  private:
	Server(int &socket);
  public:
	void    run();
	static Server& get_instance(int socket);
};
#endif
