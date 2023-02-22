class Client; // Forward declaration

class Mediator {
public:
    void registerClient(Client* client);
    void registerChannel(Channel* channel);
    void join(Client* client, Channel* channel);
    void part(Client* client, Channel* channel);
    // Add additional command methods here as needed
private:
    std::vector<Client*> clients;
    std::vector<Channel*> channels;
};

class Command {
public:
    virtual void execute(Mediator* mediator) = 0;
};

class Join : public Command {
public:
    Join(Client* client, Channel* channel) : __client(client), __channel(channel) {}
    virtual void execute(Mediator* mediator) {
        mediator->join(__client, __channel);
    }
private:
    Client* __client;
    Channel* __channel;
};

class Part : public Command {
public:
    Part(Client* client, Channel* channel) : client(client), channel(channel) {}
    virtual void execute(Mediator* mediator) {
        mediator->part(client, channel);
    }
private:
    Client* client;
    Channel* channel;
};

class Client {
public:
    Client(std::string name) : name(name) {}
    void join(Channel* channel) {
        Command* command = new Join(this, channel);
        command->execute(mediator);
        delete command;
    }
    void part(Channel* channel) {
        Command* command = new Part(this, channel);
        command->execute(mediator);
        delete command;
    }
    // Add additional command methods here as needed
private:
    std::string name;
    std::vector<Channel*> channels;
    Mediator* mediator;
    friend class Mediator;
};

class Channel {
public:
    Channel(std::string name) : name(name) {}
    void addClient(Client* client) {
        clients.push_back(client);
        client->channels.push_back(this);
    }
    void removeClient(Client* client) {
        clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
        client->channels.erase(std::remove(client->channels.begin(), client->channels.end(), this), client->channels.end());
    }
private:
    std::string name;
    std::vector<Client*> clients;
    friend class Mediator;
};

void Mediator::registerClient(Client* client) {
    clients.push_back(client);
    client->mediator = this;
}
void Mediator::registerChannel(Channel* channel) {
    channels.push_back(channel);
}
void Mediator::join(Client* client, Channel* channel) {
    channel->addClient(client);
}
void Mediator::part(Client* client, Channel* channel) {
    channel->removeClient(client);
}
