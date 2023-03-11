#include <iostream>
#include "socket.class.hpp"
#include "server.class.hpp"

int main(int ac, char **av) {
    // TODO:: add port option
    if (ac == 2) {
        Socket socket("4040");
        std::string password = std::string(av[1]);
        Server server = Server::get_instance(socket.get_socket(), password); 
        server.run();
    } else {
        std::cout << "USAGE: ./bin/ircserv PORT PASSWORD\n";
    }
}
