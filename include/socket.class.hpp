#ifndef SOCKET_CLASS_HPP
# define SOCKET_CLASS_HPP

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>


class Socket {
  private:
    const std::string __port;
    const std::string __address;
    struct addrinfo __hints;
    struct addrinfo *__res;
    int __socket;
  public:
    Socket(const std::string port);
    Socket(const std::string port, const std::string address);
    int get_socket();
    ~Socket( );
};
#endif
