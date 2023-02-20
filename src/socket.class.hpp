#ifndef SOCKET_CLASS_HPP
# define SOCKET_CLASS_HPP

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


class Socket {
  private:
    const std::string port;
    const std::string address;
    struct addrinfo hints;
    struct addrinfo *res;
    int socket;
  public:
    Socket(const std::string port, const std::string address);
    int get_socket();
    ~Socket( );
};
#endif
