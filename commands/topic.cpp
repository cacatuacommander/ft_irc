#include "../irc.hpp"

bool checkParamsTopic(const Command &cmd, const std::string &nick, int fd, std::vector<Channel> &channelVect, std::vector<User> & uservect)
{
    if (cmd.params.size() < 1)
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 461 " + nick +
                          " INVITE :Not enough parameters\r\n";
        safe_send(uservect, fd, msg);
        return false;
    }

    std::string channelName = cmd.params[0];
    size_t ch = searchChannel(channelVect, channelName);
    if (ch == channelVect.size())
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 403 " + nick +
                          " " + channelName + " :No such channel\r\n";
        safe_send(uservect, fd, msg);
        return false;
    }

    Channel &channel = channelVect[ch];
    if (!channel.userIsInChannel(fd))
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 442 " + nick +
                          " " + channelName + " :You're not on that channel\r\n";
        safe_send(uservect, fd, msg);
        return false;
    }

    if (!channel.searchFDinOperators(fd) && (!cmd.trailing.empty() || cmd.params.size() >= 2) && channel.isTopicResticted())
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 482 " + nick +
                          " " + channelName + " :You're not channel operator\r\n";
        safe_send(uservect, fd, msg);
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

void execTopic(Command cmd, int fd, std::vector<Channel>& channelVect, std::vector<User> & uservect)
{
    size_t sender_index = searchVectWithFd(uservect, fd);
    std::string nick = uservect[sender_index].getNickName().empty() ? "*" : uservect[sender_index].getNickName();
    if (!checkParamsTopic(cmd, nick, fd, channelVect, uservect))
        return ;
    size_t channel_index = searchChannel(channelVect, cmd.params[0]);
    Channel &channel = channelVect[channel_index];
    User &sender = uservect[sender_index];

    std::string msg;
    if (cmd.trailing.empty() && cmd.params.size() < 2 && cmd.ghost_trail == false)
    {
        if (channel.getTopic() == "No topic is set") 
            msg = ":" + std::string(SERVER_NAME) + " 331 " + sender.getNickName() + " " + channel.getName() + " :" + channel.getTopic() + "\r\n";
        else
            msg = ":" + std::string(SERVER_NAME) + " 332 " + sender.getNickName() + " " + channel.getName() + " :" + channel.getTopic() + "\r\n";
        safe_send(uservect, fd, msg);
    }
    else
    {
        std::string newtopic;
        if (cmd.trailing.empty())
            newtopic = cmd.params[1];
        else
            newtopic = cmd.trailing;
        channel.changeTopic(newtopic);
        msg = ":" + sender.getNickName() + " TOPIC " + channel.getName() + " :" + channel.getTopic() + "\r\n";
        channel.sendToAll(uservect, msg, fd);
        safe_send(uservect, fd, msg);
    }
}