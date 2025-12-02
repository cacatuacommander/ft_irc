#include "../irc.hpp"

bool checkCmdParams(const std::string &nick, int fd, std::vector<User> &uservect, std::vector<Channel> &channelVect, std::string t)
{
    size_t ir;
    if (t[0] != '#')
    {
        ir = searchVectWithNick(uservect, t);
        if (ir == uservect.size())
        {
            std::string msg = ":" + std::string(SERVER_NAME) + " 401 " + nick + " " + t + " :No such nick\r\n";
            safe_send(uservect, fd, msg);
            return false;
        }
    }
    else {
        ir = searchChannel(channelVect, t);
        if (ir == channelVect.size())
        {
            std::string msg = ":" + std::string(SERVER_NAME) + " 403 " + nick + " " + t + " :No such channel\r\n";
            safe_send(uservect, fd, msg);
            return false;
        }
        if (!channelVect[ir].userIsInChannel(fd))
        {
            std::string msg = ":" + std::string(SERVER_NAME) + " 404 " + nick + " " + t + " :Cannot send to channel\r\n";
            safe_send(uservect, fd, msg);
            return false;
        }
    }
    return true;
}

bool checkParamsPrvMsg(const Command &cmd, std::vector<std::string> multiParam, const std::string &nick, int fd, std::vector<User> &uservect, std::vector<Channel> &channelVect) {
    if (cmd.params.size() < 1)
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 411 " + nick + " :No recipient given (PRIVMSG)\r\n";
        safe_send(uservect, fd, msg);
        return false;
    }
    if (cmd.trailing.empty())
    {
        std::string msg = ":" + std::string(SERVER_NAME) + " 412 " + nick + " :No text to send\r\n";
        safe_send(uservect, fd, msg);
        return false;
    }
    for (size_t i = 0; i < multiParam.size(); i++)
    {
        if (!checkCmdParams(nick, fd, uservect, channelVect, multiParam[i]))
            return false;
    }
    return true;
}

void execPrivMsg(Command cmd, int fd, std::vector<Channel>& channelVect, std::vector<User> & uservect) {
    int is = searchVectWithFd(uservect, fd);
    std::string nick = uservect[is].getNickName().empty() ? "*" : uservect[is].getNickName();
    std::string username = uservect[is].getUserName().empty() ? "*" : uservect[is].getUserName();
    std::vector<std::string> multiParam;
    if (cmd.params[0].find(',') != std::string::npos)
    {
        std::string target = cmd.params[0];
        std::stringstream ss(target);
        std::string t;
        while (std::getline(ss, t, ','))
        {
            multiParam.push_back(t);
        }
    }
    else
        multiParam.push_back(cmd.params[0]);

    if (!checkParamsPrvMsg(cmd, multiParam, nick, fd, uservect, channelVect))
        return ;
    for (size_t i = 0; i < multiParam.size(); i++)
    {
        std::string target = multiParam[i];
        std::string msg = ":" + nick + "!~" + username + "@" + "127.0.0.1 " +  "PRIVMSG " + target + " :" + cmd.trailing + "\r\n";
        if (target[0] != '#')
        {
            size_t ir = searchVectWithNick(uservect, target);
            if (ir != uservect.size()) {
                send(uservect[ir].getFd(), msg.c_str(), msg.size(), 0);
            }
        }
        else
        {
            size_t ir = searchChannel(channelVect, target);
            if (ir != channelVect.size()) {
                channelVect[ir].sendToAll(uservect, msg, fd);
            }
        }
    }
}
