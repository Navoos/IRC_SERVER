#include "server.class.hpp"
#include "client.class.hpp"
#include "socket.class.hpp"
#include <iostream>
#include <map>

Server::Server(int &socket, std::string &password) : __socket(socket), __password(password) {
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


Server&  Server::get_instance(int socket, std::string &password) {
  static Server instance(socket, password);
  return instance;
}

void Server::run() {
  // server loop
  struct sockaddr_storage remote_addr;
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
		// for the server
		if (this->__fds[i].fd == this->__socket)
		{
		  client_fd = accept(this->__socket, (sockaddr *)&remote_addr, &addrlen);
		  if (client_fd < 0) {
			perror("accept");
		  } else {
			++fd_count;
			struct pollfd n;
			n.fd = client_fd;
			n.events = POLLIN;
			this->__fds.push_back(n);
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
			close(this->__fds[i].fd);
			this->__mediator->delete_client(this->__fds[i].fd);
			this->__fds.erase(this->__fds.begin() + i);
			--fd_count;
		  } else {
			std::string s_buffer(buf);
			if (this->__mediator->find_client(this->__fds[i].fd)) {
                this->__mediator->set_client(this->__fds[i].fd, s_buffer);
			} else {
				// std::cerr << "Adding new client ..." << std::endl;
                this->__mediator->add_client(this->__fds[i].fd, this->__password, s_buffer, this->__mediator);
			}
		  }
		}
	  }
	}
  }
}


std::string Server::get_password(void) const
{ return __password; }
