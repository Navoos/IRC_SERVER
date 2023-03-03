#ifndef MEDIATOR_CLASS_HPP
# define MEDIATOR_CLASS_HPP
#include <vector>
#include <map>

#define ERR_ALREADYREGISTERED "462"
#define ERR_NEEDMOREPARAMS    "461"
#define ERR_PASSWDMISMATCH    "464"
#define ERR_NONICKNAMEGIVEN    "431"
#define ERR_ERRONEUSNICKNAME   "432"
#define ERR_NICKNAMEINUSE   "433"
#include "server.class.hpp"
class Channel;
class Client;
class Server;

class Mediator {
  public:
      Server get_server();
      void  pass_cmd(Client *client, Server server);   // houssam
      void  user_cmd(Client *client);   // houssam
      void  nick_cmd(Client *client);   // houssam
      void  join_cmd(Client *client, Channel *channel);   //houssam
      void  part_cmd(Client *client, Channel *channel);   //ayoub
      void  mode_cmd(Client *client, Channel *channel);   //ayoub
      void  kick_cmd(Client *client, Channel *channel);   //ayoub
      void  quit_cmd(Client *client);   //ayoub
      void  delete_client(int fd);
      bool  find_client(int fd);
};
#endif
