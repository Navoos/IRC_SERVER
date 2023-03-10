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
	std::string __key;
    std::set<int> __moderators;
    std::set<int> __invited;
public:
    Channel(std::string name, std::string topic,std::string key);
    ~Channel();

    std::string get_name(void) const;
    std::string get_topic(void) const;
    std::string get_key(void) const;
    bool get_mode(void) const;

    void    set_name(std::string name);
    void    set_topic(std::string topic);
    void    set_key(std::string key);
    void    set_mode(bool mode);
    void    add_moderator(int client_id);
    void    add_client(Client *client);
    bool    is_invited(int client_id);
    bool    find_client(int client_id);
    bool    find_operator(int client);
    void    delete_client(int client_fd);          
    std::map<int, Client*>& get_all_client();
};

#endif