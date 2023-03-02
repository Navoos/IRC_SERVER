#ifndef SERVER_CLASS_HPP
# define SERVER_CLASS_HPP

#include <vector>
#include <string>
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
	std::vector<pollfd>     __fds;
	int                     __socket;
	std::map<int, *Client>  __clients;
    std::string			    __password;
  private:
	Server(int &socket, std::string &password);
  public:
	void    run();
    Server();
	static Server& get_instance(int socket, std::string &password);
    std::string	   get_password(void) const;
    std::map<int, Client>& get_clients(void);
};
#endif
