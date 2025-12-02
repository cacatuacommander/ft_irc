#include "../irc.hpp"

bool checkParamsBotCall(const Command &cmd, const std::string &nick, int fd, std::vector<Channel> &channelVect, std::vector<User> & uservect)
{
    if (cmd.params.size() < 1)
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 461 " + nick +
                          " BOT CALL :Channel name required\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return false;
    }

    std::string channelName = cmd.params[0];
    size_t ch = searchChannel(channelVect, channelName);
    if (ch == channelVect.size())
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 403 " + nick +
                          " " + channelName + " :No such channel\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return false;
    }

    Channel &channel = channelVect[ch];
    std::string bot_nick = "bot";
    size_t bot_idx = searchVectWithNick(uservect, bot_nick);
    if (channel.userIsInChannel(uservect[bot_idx].getFd()))
    {
        std::string msg = "The Shaman is already in the channel, STOP invoking him!\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return false;
    }

    if (!channel.userIsInChannel(fd))
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 442 " + nick +
                          " " + channelName + " :You're not on that channel\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return false;
    }

    if (!channel.searchFDinOperators(fd))
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 482 " + nick +
                          " " + channelName + " :You're not channel operator\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return false;
    }

    return true;
}
//exec_command(Command cmd, std::vector<User> & uservect, std::vector<Channel> & channelvect, std::string serverpassword, std::vector<pollfd> &fds, int i)
void execBotCall(Command cmd, int fd, std::vector<Channel>& channelVect, std::vector<User> & uservect) {
    size_t sender_index = searchVectWithFd(uservect, fd);
    std::string nick = uservect[sender_index].getNickName().empty() ? "*" : uservect[sender_index].getNickName();
    if (!checkParamsBotCall(cmd, nick, fd, channelVect, uservect))
        return ;

    size_t channel_index = searchChannel(channelVect, cmd.params[0]);
    Channel &channel = channelVect[channel_index];
    std::string msg = "The shaman was invoked... Let's wait for that old geezer\r\n";

    channel.sendToAll(msg, fd);
    send(fd, msg.c_str(), msg.size(), 0);

    std::string bot_nick = "bot";
    std::string bot_msg = "BOT :JOIN " + channel.getName() + "\r\n";
    size_t bot_idx = searchVectWithNick(uservect, bot_nick);

    if (bot_idx < uservect.size())
        send(uservect[bot_idx].getFd(), bot_msg.c_str(), bot_msg.size(), 0);
    else
        msg = "The shaman is not collaborating\r\n";
}
