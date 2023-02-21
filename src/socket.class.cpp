#include "socket.class.hpp"

Socket::Socket(const std::string port) : __port(port) {
  memset(&this->__hints, 0, sizeof this->__hints);
  this->__res = 0x0;
  this->__hints.ai_family = AF_UNSPEC;
  this->__hints.ai_socktype = SOCK_STREAM;
  this->__hints.ai_flags = AI_PASSIVE;
  if (getaddrinfo(0x0, this->__port.c_str(), &(this->__hints), &(this->__res)) != 0)
    exit(EXIT_FAILURE);
  this->__socket = socket(this->__res->ai_family, this->__res->ai_socktype, this->__res->ai_protocol);
  if (this->__socket == -1)
    exit(EXIT_FAILURE);
}

Socket::Socket(const std::string port, const std::string address) : __port(port) {
  memset(&this->__hints, 0, sizeof this->__hints);
  this->__res = 0x0;
  this->__hints.ai_family = AF_UNSPEC;
  this->__hints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo(address.c_str(), this->__port.c_str(), &(this->__hints), &(this->__res)) != 0)
    exit(EXIT_FAILURE);
  this->__socket = socket(this->__res->ai_family, this->__res->ai_socktype, this->__res->ai_protocol);
  if (this->__socket == -1)
    exit(EXIT_FAILURE);
}

int Socket::get_socket() {
  return this->__socket;
}

Socket::~Socket() {
  freeaddrinfo(this->__res);
}
