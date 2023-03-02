#ifndef MEDIATOR_CLASS_HPP
# define MEDIATOR_CLASS_HPP
#include <vector>

#define ERR_ALREADYREGISTERED "462"
#define ERR_NEEDMOREPARAMS    "461"
#define ERR_PASSWDMISMATCH    "464"
#define ERR_NONICKNAMEGIVEN    "431"
#define ERR_ERRONEUSNICKNAME   "432"
#define ERR_NICKNAMEINUSE   "433"

class Client;
class Channel;
class Server;

class Mediator {
  public:
      void  register_client(Client *client);
      void  register_channel(Channel *channel);
      Server* get_server();
      void  pass_cmd(Client *client, Server *servre);   // houssam
      void  user_cmd(Client *client);   // houssam
      void  nick_cmd(Client *client);   // houssam
      void  join_cmd(Client *client, Channel *channel);   //houssam
      void  part_cmd(Client *client, Channel *channel);   //ayoub
      void  mode_cmd(Client *client, Channel *channel);   //ayoub
      void  kick_cmd(Client *client, Channel *channel);   //ayoub
      void  quit_cmd(Client *client);   //ayoub
      // todo: add invit command
      // todo: add topic command
      // HOUSSAM: add additional commands here
  private:
      std::vector<Client*> __clients;
      std::vector<Channel*> __channels;
      Server *__server;
};

#endif
