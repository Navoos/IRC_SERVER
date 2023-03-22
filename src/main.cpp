#include <iostream>
#include "socket.class.hpp"
#include "server.class.hpp"

int main(int ac, char **av) {
    if (ac == 3) {
        std::string port = std::string(av[1]);
        Socket socket(port);
        std::string password = std::string(av[2]);
        Server server = Server::get_instance(socket.get_socket(), password, port);
        server.run();
    } else {
        std::cout << "USAGE: ./bin/ircserv PORT PASSWORD\n";
    }
}
