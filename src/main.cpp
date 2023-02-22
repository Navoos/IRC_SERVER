#include "main.hpp"

int main() {
  Socket socket("4040");
  Server server = Server::get_instance(socket.get_socket());
  server.run();
}
