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

class Mediator;
class Server {
  private:
	std::vector<pollfd>     __fds;
	int                     __socket;
    std::string			    __password;
    std::string         __port;
    Mediator                *__mediator;

  private:
	Server(int &socket, std::string &password, std::string &port);
  public:
    Server();
    ~Server();
	void    run();
	static Server& get_instance(int socket, std::string &password, std::string &port);
    std::string	   get_password(void) const;
    void check_for_quitter(Mediator *mediator, int &fd_count, int index);
};
#endif
