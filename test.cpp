void Mediator::privmsg_cmd(Client *client) {
    if (client->__cmd.size() < 3) {
        std::string msg = ":ft_irc 461 " + client->get_nickname() + " :Not enough parameters";
        client->put_message(msg);
        return ;
    }
    std::vector<std::string> targets = split(client->__cmd[1], ',');
    std::string message_to_be_sent = client->__cmd[2][0] == ':' ? client->__cmd[2].substr(1) : client->__cmd[2]; 
    for (unsigned int i = 3; i < client->__cmd.size();++i) {
        message_to_be_sent += client->__cmd[i];
        message_to_be_sent += " ";
    }

    for (unsigned int i = 0;i < targets.size();++i) {
        if (targets[i][0] == '#') {
            if (this->__channels.find(targets[i]) == this->__channels.end()) {
                std::string msg = ":ft_irc 403 " + client->get_nickname() + " " + client->__cmd[1] + " :No such channel";
                client->put_message(msg);
                continue ;
            } else {
                // if channel is moderated the only who has the voice command and operator can send messages
                Channel *channel = this->__channels.at(targets[i]);
                if (!channel)
                    continue;
                if (!channel->is_moderated()) {
                    // send normally
                    std::string msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " PRIVMSG " + channel->get_name() + " " + message_to_be_sent + "\r\n";
                    for (std::map<int, Client *>::iterator it = channel->get_all_client().begin(); it != channel->get_all_client().end();++it) {
                        if (send(it->first, msg.c_str(), msg.length(), 0) == -1)
                        {
                            perror("send");
                            continue;
                        }
                    }
                    continue;
                }
                if (channel->is_moderated() && (channel->find_operator(client->get_socket()) || client->has_voice())) {
                    // send message to all clients currently connected
                    for (std::map<int, Client *>::iterator it = channel->get_all_client().begin(); it != channel->get_all_client().end();++it) {
                        if (send(it->first, client->__cmd[2].c_str(), client->__cmd[2].length(), 0) == -1)
                        {
                            perror("send");
                            continue;
                        }
                    }
                    continue;
                } else {
                    std::string msg = ":ft_irc 404 " + client->get_nickname() + " " + targets[i] + " :Cannot send to channel";
                    client->put_message(msg);
                    continue ;
                }
            }
       } else {
           Client *send_client = get_client(targets[i]);
           if (send_client) {
                std::string msg = ":" + client->get_nickname() + "!" + client->get_username() + "@" + client->get_hostname() + " PRIVMSG " + send_client->get_nickname() + " " + message_to_be_sent + "\r\n";
                if (send(send_client->get_socket(), msg.c_str(), msg.length(), 0) == -1) {
                    perror("send");
                    continue;
                }
                continue;
           } else {
                std::string msg = ":ft_irc 401 " + client->get_nickname() + " " + targets[i] + " :No such nick";
                client->put_message(msg);
                continue ;
           }
       }
    }
}