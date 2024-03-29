#ifndef CLIENT_CLASS_HPP
# define CLIENT_CLASS_HPP
#include <vector>
#include <string>
#include <iostream>
#include "server.class.hpp"
#include "mediator.class.hpp"
#include <sys/socket.h>

class Client {
	private:
		std::vector<std::string>		__cmd;
		std::string						&__server_password;
		std::map<std::string, Channel*>	__channels;
		std::string						__buffer;
		int								__fd;
		Mediator						*__mediator;
		friend class 					Mediator;
		bool							__connected;
		bool							__accepted;
		std::string						__nick;
		std::string						__user;
        bool                            __voice;
        std::string                     __hostname;
	public:
		Client(int fd, std::string &password, Mediator *mediator);
		Client(int fd, std::string &password, Mediator *mediator, struct sockaddr &addr);
		bool    		is_connected();
		bool    		is_accepted(); 
		bool    		put_message(std::string message);
		void    		update_client(std::string &str);
		void    		set_accepted(bool acc);
		void    		set_connected(bool connected);
		void			set_nickname(std::string nick);
		void			set_username(std::string user);
		std::string	    get_nickname(void) const ;
		std::string	    get_username(void) const ;
    	bool    		check_connection(void);
    	void    		execute(Mediator *mediator);
    	void    		subscribe_to_channel(Channel *channel);
    	int				get_socket(void) const;
    	Channel*    	get_channel(std::string &name);
        std::map<std::string, Channel *> &get_all_channels();
        bool            has_voice();
		void			erase_channel(std::string &channel);
        std::string     get_hostname();
};

#endif
