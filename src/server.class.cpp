#include "server.class.hpp"
#include <iostream>

Server::Server(int &socket) : __socket(socket) {
  fcntl(this->__socket, F_SETFL, O_NONBLOCK);
  if (listen(this->__socket, BACKLOG) == -1)
    exit(EXIT_FAILURE);
  this->__fds.push_back(pollfd());
  this->__fds[0].fd = this->__socket;
  this->__fds[0].events = POLLIN;
}

Server&  Server::get_instance(int socket) {
  static Server instance(socket);
  return instance;
}

void Server::run() {
  // server loop
  int fd_count = 1;
  socklen_t addrlen;
  struct sockaddr remote_addr;
  char  ip4[INET_ADDRSTRLEN];
  addrlen = sizeof remote_addr;
  int   client_fd;
  for (;;) {
    int p = poll(this->__fds.data(), fd_count, 0);
    if (p == -1)
    {
      perror("poll");
      exit(EXIT_FAILURE);
    }
    for (int i = 0;i < fd_count;++i) {
      if (this->__fds[i].revents & POLLIN) {
        // for the server
        if (this->__fds[i].fd == this->__socket)
        {
          client_fd = accept(this->__socket, &remote_addr, &addrlen);
          if (client_fd < 0) {
            perror("accept");
          } else {
            struct pollfd n;
            n.fd = client_fd;
            n.events = POLLIN;
            this->__fds.push_back(n);
            std::cout << "New client connected from: " << inet_ntop(AF_INET,  &((struct sockaddr_in *)(&remote_addr))->sin_addr, ip4, INET_ADDRSTRLEN) << std::endl;
          }
        }
      }
    }
  }
}
