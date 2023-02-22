#ifndef SERVER_CLASS_HPP
# define SERVER_CLASS_HPP

#include "socket.class.hpp"

class Server {
  private:
    Socket  &__socket;
    Server(Socket &socket);
  public:
    void    bind();
    void    listen();
    void    run();
};
#endif
