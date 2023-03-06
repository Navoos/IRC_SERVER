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
      void  part_cmd(Client *client, std::vector<Channel*> channel, std::vector<std::string> __cmd);   //ayoub
      void  mode_cmd(Client *client, Channel *channel);   //ayoub
      void  kick_cmd(Client *client, Channel *channel);   //ayoub
      void  quit_cmd(Client *client);   //ayoub
      void  delete_client(int fd);
      bool  find_client(int fd);
      void  set_client(int fd, std::string &buffer);
      void  add_client(int fd, std::string &password, std::string &buffer, Mediator *mediator);
      //Deadpool
      std::vector<Channel*> get_channels();
      void  add_channel();
      std::vector<std::string> split(const std::string& input, const std::string& delimiter);
      // ******************************** //
      // todo: add invit command
      // todo: add topic command
      // HOUSSAM: add additional commands here
      Mediator(Server& server);
  private:
      std::map<int, Client*>    __clients;
      std::vector<Channel*>     __channels;
      Server                    __server;
};
#endif



// static int	count_words(const char *str, char c)
// {
// 	int	i;
// 	int	count;

// 	i = 0;
// 	count = 0;
// 	while (*str)
// 	{
// 		if (*str != c && count == 0)
// 		{
// 			count = 1;
// 			i++;
// 		}
// 		else if (*str == c)
// 			count = 0;
// 		str++;
// 	}
// 	return (i);
// }

// static char	**ft_freeing(char **tab)
// {
// 	int	i;

// 	i = 0;
// 	while (tab[i])
// 		free(tab[i++]);
// 	free(tab);
// 	return (NULL);
// }

// static char	**set_words(char const *s, char c)
// {
// 	int		i;
// 	int		j;
// 	int		k;
// 	char	**tab;

// 	tab = (char **)malloc((count_words(s, c) + 1) * sizeof(char *));
// 	if (!tab)
// 		return (NULL);
// 	i = 0;
// 	k = 0;
// 	while (s[i])
// 	{
// 		while (s[i] == c && s[i])
// 			i++;
// 		j = 0;
// 		while (s[i + j] != c && s[i + j])
// 			j++;
// 		if (j != 0)
// 			tab[k++] = ft_substr(s, i, j);
// 		if (j != 0 && !tab[k - 1])
// 			return (ft_freeing(tab));
// 		i = i + j;
// 	}
// 	tab[k] = NULL;
// 	return (tab);
// }

// char	**ft_split(char const *s, char c)
// {
// 	char	**tab;

// 	if (!s)
// 		return (NULL);
// 	tab = set_words((char *)s, c);
// 	return (tab);
// }
