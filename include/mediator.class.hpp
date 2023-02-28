#ifndef MEDIATOR_CLASS_HPP
# define MEDIATOR_CLASS_HPP
#include <vector>

class Client;
class Channel;

class Mediator {
  public:
      void  register_client(Client *client);
      void  register_channel(Channel *channel);
      void  pass_cmd(Client *client);   // houssam
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
};

#endif
