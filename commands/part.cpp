#include "../irc.hpp"

bool checkParamsPart(const Command &cmd, const std::string &nick, int fd, std::vector<Channel> &channels, std::vector<User> &uservect)
{
    if (cmd.params.empty())
    {
        std::string msg = ":" + std::string(SERVER_NAME) +
                          " 461 " + nick + " PART :Not enough parameters\r\n";
        safe_send(uservect, fd, msg);
        return false;
    }

    std::string channelName = cmd.params[0];

    size_t ch_idx = searchChannel(channels, channelName);
    if (ch_idx == channels.size())
    {
        std::string msg = ":" + std::string(SERVER_NAME) +
                          " 403 " + nick + " " + channelName + " :No such channel\r\n";
        safe_send(uservect, fd, msg);
        return false;
    }

    //size_t usr_idx = searchVectWithFd(users, fd);
    if (!channels[ch_idx].userIsInChannel(fd))
    {
        std::string msg = ":" + std::string(SERVER_NAME) +
                          " 442 " + nick + " " + channelName +
                          " :You're not on that channel\r\n";
        safe_send(uservect, fd, msg);
        return false;
    }

    return true;
}

void execPart(Command & cmd, int fd,  std::vector<User>& users, std::vector<Channel>& channels)
{
    size_t usr_idx = searchVectWithFd(users, fd);
    std::string nick = users[usr_idx].getNickName().empty() ? "*" : users[usr_idx].getNickName();

    if (!checkParamsPart(cmd, nick, fd, channels, users))
        return;

    std::string channelName = cmd.params[0];
    size_t ch_idx = searchChannel(channels, channelName);

    Channel &chan = channels[ch_idx];
    //User &usr = users[usr_idx];
    std::string trailing = cmd.trailing.empty() ? nick : cmd.trailing;
    std::string msg = ":" + nick + " PART " + channelName + " :" + trailing + "\r\n";
    chan.sendToAll(users, msg, fd);
    chan.removeUser(fd);
    if (channels[ch_idx].getUsersSize() == 0 || (channels[ch_idx].getUsersSize() == 1 && channels[ch_idx].getListOfNicks(users) == "@bot"))
    	channels.erase(channels.begin() + ch_idx);
    safe_send(users, fd, msg);
}

