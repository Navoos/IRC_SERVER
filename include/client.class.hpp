#ifndef CLIENT_CLASS_HPP
# define CLIENT_CLASS_HPP
#include <vector>
#include <string>
#include "server.class.hpp"
#include "mediator.class.hpp"

class Client {
  private:
    std::vector<std::string> __cmd;
    std::string             &__server_password;
	std::string   __buffer;
	  int           __fd;
    Mediator *__mediator;
    friend class Mediator;

    bool    __connected;
    bool    __accepted;

	std::string				__nick;
	std::string				__user;
  
  public:
    Client(int fd, std::string &password, Mediator *mediator);
    bool    is_connected();
    bool    is_accepted(); 
    void    put_message(std::string name, std::string message);
    void    update_client(std::string &str);
    void    set_accepted(bool acc);
    void    set_connected(bool connected);
    void	set_nickname(std::string nick);
	void	set_username(std::string user);
    std::string	    get_nickname(void) const ;
	std::string	    get_username(void) const ;
    bool    check_connection(void);
    void    execute(Mediator *mediator);

};
#endif
