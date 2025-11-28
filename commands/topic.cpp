#include "../irc.hpp"

bool checkParamsTopic(const Command &cmd, const std::string &nick, int fd, std::vector<Channel> &channelVect)
{
    if (cmd.params.size() < 1)
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 461 " + nick +
                          " INVITE :Not enough parameters\r\n";
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
    if (!channel.userIsInChannel(fd))
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 442 " + nick +
                          " " + channelName + " :You're not on that channel\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return false;
    }

    if (!channel.searchFDinOperators(fd) && !cmd.trailing.empty())
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 482 " + nick +
                          " " + channelName + " :You're not channel operator\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return false;
    }

    return true;
}

// TOPIC #music ->
//    - :server 332 <yourNick> #music :Benvenuti nel canale!
//    - :server 331 <yourNick> #music :No topic is set
//
// TOPIC #music :oggi si parla di produzione musicale
//    - :<yourNick> TOPIC #music :oggi si parla di produzione musicale

void execTopic(Command cmd, int fd, std::vector<Channel>& channelVect, std::vector<User> & uservect) {
    size_t sender_index = searchVectWithFd(uservect, fd);
    std::string nick = uservect[sender_index].getNickName().empty() ? "*" : uservect[sender_index].getNickName();
    if (!checkParamsTopic(cmd, nick, fd, channelVect))
        return ;
    size_t channel_index = searchChannel(channelVect, cmd.params[0]);
    Channel &channel = channelVect[channel_index];
    User &sender = uservect[sender_index];

    std::string msg;
    if (cmd.trailing.empty() && cmd.ghost_trail == false)
    {
        if (channel.getTopic() == "No topic is set") 
            msg = ":" + std::string(SERVER_NAME) + " 331 " + sender.getNickName() + " " + channel.getName() + " :" + channel.getTopic() + "\r\n";
        else
            msg = ":" + std::string(SERVER_NAME) + " 332 " + sender.getNickName() + " " + channel.getName() + " :" + channel.getTopic() + "\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
    }
    else
    {
        channel.changeTopic(cmd.trailing);
        msg = ":" + sender.getNickName() + " TOPIC " + channel.getName() + " :" + channel.getTopic() + "\r\n";
        channel.sendToAll(msg, fd);
    }
}