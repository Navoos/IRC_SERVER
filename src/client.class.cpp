#include "client.class.hpp"

bool		Client::isConnected(void) { 
    return __connecte; }

void Client::put_message(std::string name, std::string message)
{
    std::stringstream msg;

    msg  << ":IRC_SERVER " << name << " " << message;

    std::cout << msg.str() << std::endl;
}