#include "irc.hpp"
#include <sstream>

bool Parser::cmd_exist(const std::string& cmd) {
    std::string cmd_list[] = {"PING / PONG", "JOIN / PART", "PRIVMSG", "QUIT", "MODE", "KICK", "INVITE", "TOPIC", "PASS", "NICK", "USER"};
    for (int i = 0; i < 9; i++)
    {
        if (cmd == cmd_list[i])
            return true;
    }
    return false;
}

std::vector<User>::const_iterator userResearch(int fd, const std::vector<User>& usr_vec) {
    std::vector<User>::const_iterator it;
    for (it = usr_vec.begin(); it != usr_vec.end(); it++)
    {
        if (it->getFd() == fd)
            return it;
    }
    return usr_vec.end();
}

Command Parser::parse(const std::string& input, const std::vector<User>& usr_vec, const int fd) {
    Command cmd;
    cmd.valid = false;
    std::string tmp = input;

    std::vector<User>::const_iterator curr_usr = userResearch(fd, usr_vec);
    if (curr_usr == usr_vec.end())
        return cmd;

    std::string nick = curr_usr->getNickName().empty() ? "*" : curr_usr->getNickName();
    if (tmp.size() < 4 || tmp[tmp.size() - 1] != '\n' || tmp[tmp.size() - 2] != '\r') {
        std::string err = ":" + std::string(SERVER_NAME) + " 421 " + nick + " :Unknown command babo\r\n";
    	send(fd, err.c_str(), err.size(), 0);
        return cmd;
    }
    else
        tmp.erase(tmp.size()-2, 2);

    size_t pos = tmp.find(" :");
    if (pos != std::string::npos)
    {
        cmd.trailing = tmp.substr(pos + 2);
        tmp = tmp.substr(0, pos);
    }
    std::string tmpCmd;
    std::stringstream ss(tmp);
    ss >> tmpCmd;
    if (cmd_exist(tmpCmd) == false) {
        std::string err = ":" + std::string(SERVER_NAME) + " 421 " + nick + " :Unknown command\r\n";
    	send(fd, err.c_str(), err.size(), 0);
        return cmd;
    }
    else {
        std::vector<User>::const_iterator curr_usr = userResearch(fd, usr_vec);
        if (curr_usr == usr_vec.end())
            return cmd;
        if (tmpCmd == "NICK") {
            if (!curr_usr->getPassword()) {
                std::string msg = ":" + std::string(SERVER_NAME) + " 451 * :You have not registered\r\n";
                send(fd, msg.c_str(), msg.size(), 0);
                return cmd;
            }
        }
        else if (tmpCmd == "USER") {
            if (curr_usr->getPassword() == false || curr_usr->getNickName() == "") {
                std::string msg = ":" + std::string(SERVER_NAME) + " 451 * :You have not registered\r\n";
		        send(fd, msg.c_str(), msg.size(), 0);
                return cmd;
            }
        }
        else {
            if (curr_usr->getUserName() == "" && tmpCmd != "PING / PONG" && tmpCmd != "QUIT" && tmpCmd != "PASS") {
                std::string msg = ":" + std::string(SERVER_NAME) + " 451 " + nick + " :You have not registered\r\n";
		        send(fd, msg.c_str(), msg.size(), 0);
                return cmd;
            }
        }
    }

    cmd.name = tmpCmd;

    std::string arg;
    while (ss >> arg)
        cmd.params.push_back(arg);
    
    cmd.valid = true;
    return cmd;
}
