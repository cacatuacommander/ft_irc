#include "../irc.hpp"

bool checkParamsPrvMsg(const Command &cmd, const std::string &nick, int fd, std::vector<User> &uservect, std::vector<Channel> &channelVect) {
    if (cmd.params.size() < 1)
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 411 " + nick + " :No recipient given (PRIVMSG)\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return false;
    }
    if (cmd.trailing.empty())
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 412 " + nick + " :No text to send\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return false;
    }

    size_t ir;
    std::string target = cmd.params[0];
    if (target[0] != '#')
    {
        ir = searchVectWithNick(uservect, target);
        if (ir == uservect.size())
        {
            std::string msg = ":" + std::string(SERVER_NAME) + " 401 " + nick + " " + target + " :No such nick\r\n";
            send(fd, msg.c_str(), msg.size(), 0);
            return false;
        }
    }
    else {
        ir = searchChannel(channelVect, target);
        if (ir == channelVect.size())
        {
            std::string msg = ":" + std::string(SERVER_NAME) + " 403 " + nick + " " + target + " :No such channel\r\n";
            send(fd, msg.c_str(), msg.size(), 0);
            return false;
        }
        if (!channelVect[ir].userIsInChannel(fd))
        {
            std::string msg = ":" + std::string(SERVER_NAME) + " 404 " + nick + " " + target + " :Cannot send to channel\r\n";
            send(fd, msg.c_str(), msg.size(), 0);
            return false;
        }
    }

    return true;
}

void execPrivMsg(Command cmd, int fd, std::vector<Channel>& channelVect, std::vector<User> & uservect)
{
    int is = searchVectWithFd(uservect, fd);
    std::string nick = uservect[is].getNickName().empty() ? "*" : uservect[is].getNickName();
    if (!checkParamsPrvMsg(cmd, nick, fd, uservect, channelVect))
        return ;
    std::string target = cmd.params[0];
    std::string msg = ":" + nick + " PRIVMSG " + target + " :" + cmd.trailing + "\r\n";
    if (target[0] != '#')
    {
        size_t ir = searchVectWithNick(uservect, target);
        if (ir != uservect.size()) {
            send(uservect[ir].getFd(), msg.c_str(), msg.size(), 0);
        }
        return;
    }
    else
    {
        size_t ir = searchChannel(channelVect, target);
        if (ir == channelVect.size()) {
            //sendtoall che devo ancora fare
            return ;
        }
    }
}