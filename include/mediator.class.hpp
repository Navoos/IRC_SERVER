#ifndef MEDIATOR_CLASS_HPP
# define MEDIATOR_CLASS_HPP
#include <vector>
#include <map>
#include <string>

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
      void  join_cmd(Client *client);   //houssam
      void  part_cmd(Client *client, Channel *channel);   //ayoub
      void  mode_cmd(Client *client, Channel *channel);   //ayoub
      void  kick_cmd(Client *client, Channel *channel);   //ayoub
      void  quit_cmd(Client *client);   //ayoub
      void  delete_client(int fd);
      bool  find_client(int fd);
      void  set_client(int fd, std::string &buffer);
      void  add_client(int fd, std::string &password, std::string &buffer, Mediator *mediator);
      void  topic_cmd(Client *client);
      bool  search_channel(std::string name, std::map<std::string, Channel*>     __channels);
      // todo: add invit command
      // todo: add topic command
      // HOUSSAM: add additional commands here
      Mediator(Server& server);
  private:
      std::map<int, Client*>                __clients;
      std::map<std::string, Channel*>     __channels;
      Server                    __server;
};
#endif
