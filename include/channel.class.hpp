#ifndef CHANNEL_CLASS_HPP
# define CHANNEL_CLASS_HPP
#include <vector>
#include <map>
#include <string>
#include "server.class.hpp"
#include "mediator.class.hpp"
#include <set>

class Channel
{
private:
	std::string __name;
	std::string __topic;
	std::map<int, Client *> __clients;
	bool __mode;
    bool __moderated;
	bool __modeinvite;
	bool __modetopic;
	bool __modekey;
	bool __modeoperator;
	std::string __key;
	std::set<int> __moderators;
	std::set<int> __invited;
public:
	Channel(std::string name, std::string topic,std::string key);
	~Channel();

	std::string				get_name(void) const;
	std::string				get_topic(void) const;
	std::string				get_key(void) const;
	bool					get_mode(void) const;
	void					set_name(std::string name);
	void					set_topic(std::string topic);
	void					set_key(std::string key);
	void					set_mode(bool mode);
	void					add_moderator(int client_id);
	void					add_client(Client *client);
	bool					is_invited(int client_id);
	bool					find_client(int client_id);
	bool					find_client(std::string &nick_name);
	bool					find_operator(int client);
    bool                    find_invited(int client);
	void					delete_client(int client_fd); 
    bool                    is_moderated();
	void					delete_moderator(int moderator);
    void                    delete_invited(int invited);
	std::set<int>			get_moderators();
	std::map<int, Client*>&	get_all_client();
	//deadpool
	void					set_modeinvite(bool invite);
	void					set_modetopic(bool topic);
	void					set_modekey(bool key);
	void        			set_modeoperator(bool oper);
	bool					get_modetopic();
	bool					get_modeinvite();
	int             		get_client(std::string nick);
};

#endif
