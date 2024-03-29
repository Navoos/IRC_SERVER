#include "server.class.hpp"
#include "client.class.hpp"
#include "socket.class.hpp"
#include <iostream>
#include <map>

Server::Server(int &socket, std::string &password, std::string &port) : __socket(socket), __password(password), __port(port) {
    this->__mediator = new Mediator(*this);
    fcntl(this->__socket, F_SETFL, O_NONBLOCK);
    if (listen(this->__socket, BACKLOG) == -1)
        exit(EXIT_FAILURE);
    this->__fds.push_back(pollfd());
    this->__fds[0].fd = this->__socket;
    this->__fds[0].events = POLLIN;
}

Server::~Server() {
    if (this->__mediator) {
        delete this->__mediator;
    }
}


Server&  Server::get_instance(int socket, std::string &password, std::string &port) {
  static Server instance(socket, password, port);
  return instance;
}

void Server::check_for_quitter(Mediator *mediator, int &fd_count, int index) {
    if (mediator->get_quit() == -1)
        return;
    close(mediator->get_quit());
    --fd_count;
    mediator->delete_client(this->__fds[index].fd);
    this->__fds.erase(this->__fds.begin() + index);
    mediator->set_quit(-1, "");
}

void Server::run() {
  struct sockaddr remote_addr;
  socklen_t addrlen = sizeof remote_addr;
  int fd_count = 1;
  int client_fd = -1;
  std::map<int, std::string> buffers;
  char buf[256];
  for (;;) {
	int p = poll(this->__fds.data(), fd_count, 0);
	if (p == -1)
	{
	  perror("poll");
	  exit(EXIT_FAILURE);
	}
	for (int i = 0;i < fd_count;++i) {
	  if (this->__fds[i].revents & POLLIN) {
		if (this->__fds[i].fd == this->__socket)
		{
          addrlen = sizeof remote_addr;
		  client_fd = accept(this->__socket, &remote_addr, &addrlen);
		  if (client_fd < 0) {
			perror("accept");
		  } else {
			fcntl(client_fd, F_SETFL, O_NONBLOCK);
			++fd_count;
			struct pollfd n;
			n.fd = client_fd;
			n.events = POLLIN;
			this->__fds.push_back(n);
            this->__mediator->add_client(client_fd, this->__password, this->__mediator, remote_addr);
		  }
		} else {
		  memset(buf, 0, sizeof(buf));
		  int bytes = recv(this->__fds[i].fd, buf, sizeof buf, 0);
		  if (bytes <= 0) {
			if (bytes == 0) {
			  std::cout << "server: socket " << this->__fds[i].fd << " hang up" << std::endl;
			} else {
			  perror("recv");
			}
            this->__mediator->set_quit(this->__fds[i].fd, "");
		  } else {
			std::string s_buffer(buf);
            this->__mediator->set_client(this->__fds[i].fd, s_buffer);
		  }
		}
	  }
      check_for_quitter(this->__mediator, fd_count, i);
	}
  }
}


std::string Server::get_password(void) const
{ return __password; }
