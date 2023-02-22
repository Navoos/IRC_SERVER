#ifndef SERVER_CLASS_HPP
# define SERVER_CLASS_HPP

#include "socket.class.hpp"
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#define BACKLOG 10

class Client;

class Server {
  private:
    std::vector<pollfd> __fds;
    int                 __socket;
    std::vector<Client*> __clients;
  private:
    Server(int &socket);
  public:
    void    run();
    static Server& get_instance(int socket);
};
#endif
