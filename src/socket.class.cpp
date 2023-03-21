#include "socket.class.hpp"

Socket::Socket(const std::string port) : __port(port) {
  struct addrinfo *res, hints, *i;
  int reusable = 1;
  memset(&hints, 0, sizeof hints);
  res = 0x0;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if (getaddrinfo(0x0, this->__port.c_str(), &(hints), &(res)) != 0)
    exit(EXIT_FAILURE);
  for (i = res; i != NULL; i = i->ai_next) {
    this->__socket = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
    if (this->__socket < 0)
      continue;
    // for the already in use message
    setsockopt(this->__socket, SOL_SOCKET, SO_REUSEADDR, &reusable, sizeof reusable);
    if (bind(this->__socket, i->ai_addr, i->ai_addrlen) < 0) {
      close(this->__socket);
      continue;
    }
    break;
  }
  freeaddrinfo(res);
  if (i == NULL)
    exit(EXIT_FAILURE);
}

Socket::Socket(const std::string port, const std::string address) : __port(port), __address(address) {
  struct addrinfo *res, hints, *i;
  int reusable = 1;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(this->__address.c_str(), this->__port.c_str(), &hints, &res) != 0)
    exit(EXIT_FAILURE);
  for (i = res; i != NULL; i = i->ai_next) {
    this->__socket = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
    if (this->__socket < 0)
      continue;
    // for the already in use message
    setsockopt(this->__socket, SOL_SOCKET, SO_REUSEADDR, &reusable, sizeof reusable);
    if (bind(this->__socket, i->ai_addr, i->ai_addrlen) < 0) {
      perror("bind");
      close(this->__socket);
      continue;
    }
    break;
  }
  freeaddrinfo(res);
  if (i == NULL)
    exit(EXIT_FAILURE);
}

int Socket::get_socket() {
  return this->__socket;
}

Socket::~Socket() {
}
