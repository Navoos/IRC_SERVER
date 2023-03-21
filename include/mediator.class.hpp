#ifndef MEDIATOR_CLASS_HPP
# define MEDIATOR_CLASS_HPP
#include <vector>
#include <map>
#include <string>
#include <sys/socket.h>

#define ERR_ALREADYREGISTERED "462"
#define ERR_NEEDMOREPARAMS    "461"
#define ERR_PASSWDMISMATCH    "464"
#define ERR_NONICKNAMEGIVEN    "431"
#define ERR_ERRONEUSNICKNAME   "432"
#define ERR_NICKNAMEINUSE   "433"
#define ERR_RESTRICTED   "018"
#define RPL_WELCOME  "001"
#define ERR_BADCHANMASK  "476"
#define ERR_INVITEONLYCHAN "473"
#define ERR_BADCHANNELKEY "475"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_CHANOPRIVSNEEDED "482"
#define ERR_NOTONCHANNEL "442"
#include "server.class.hpp"
class Channel;
class Client;
class Server;

class Mediator {
  public:
      Server get_server();
      void pass_cmd(Client *client, Server server);   // houssam
      void  user_cmd(Client *client);   // houssam
      void  nick_cmd(Client *client);   // houssam
      void  join_cmd(Client *client);   // houssam
      void  notice_cmd(Client *client);
      // deadpool
        // std::vector<std::string> split(const std::string& input, const std::string& delimiter);
        void part_cmd(Client *client);
        std::map<std::string, Channel*>& get_channels();
    //   void  part_cmd(Client *client, Channel *channel);   //ayoub
      void  command_not_found(Client *client);
      void  mode_cmd(Client *client);   //ayoub
      void  kick_cmd(Client *client);   //ayoub
      void  quit_cmd(Client *client);   //ayoub
      void  topic_cmd(Client *client);
      void  invite_cmd(Client *client);
      void  privmsg_cmd(Client *client);
      void  delete_client(int fd);
      Client*   get_client(std::string &nick_name);
      bool  find_client(int fd);
      bool  find_client(std::string &nick_name);
      void  set_client(int fd, std::string &buffer);
      void  add_client(int fd, std::string &password, std::string &buffer, Mediator *mediator);
      void  add_client(int fd, std::string &password, Mediator *mediator, struct sockaddr &addr);
      bool  search_channel(std::string name, std::map<std::string, Channel*>     __channels);
      void  notify_clients_of_new_member(Channel *channel, Client *client);
      std::string get_random_joke();
      void command_bot(Client *client);
      // todo: add invit command
      // todo: add topic command
      // HOUSSAM: add additional commands here
      Mediator(Server& server);
  private:
      std::map<int, Client*>            __clients;
      std::map<std::string, Channel*>   __channels;
      Server                            __server;
};
#endif
