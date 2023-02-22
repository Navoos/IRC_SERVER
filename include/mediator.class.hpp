#ifndef MEDIATOR_CLASS_HPP
# define MEDIATOR_CLASS_HPP
#include <vector>

class Client;
class Channel;

class Mediator {
  public:
      void  register_client(Client *client);
      void  register_channel(Channel *channel);
      void  pass_cmd(Client *client);
      void  user_cmd(Client *client);
      void  nick_cmd(Client *client);
      void  join_cmd(Client *client, Channel *channel);
      void  part_cmd(Client *client, Channel *channel);
      void  mode_cmd(Client *client, Channel *channel);
      void  kick_cmd(Client *client, Channel *channel);
      void  quit_cmd(Client *client);
      // HOUSSAM: add additional commands here
  private:
      std::vector<Client*> __clients;
      std::vector<Channel*> __channels;
};
#endif
