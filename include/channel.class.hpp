#ifndef CHANNEL_CLASS_HPP
# define CHANNEL_CLASS_HPP

#include <vector>
#include <iostream>
#include <string>

class Channel {
    private:
        std::string __name;
    public:
        Channel(std::string name);
        std::string get_channel_name();
        ~Channel();
};

#endif
