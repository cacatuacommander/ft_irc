#include "../irc.hpp"

void execPing(Command cmd, int fd, std::vector<User> & uservect)
{
	size_t i = searchVectWithFd(uservect, fd);

	std::string nickname = uservect[i].getNickName();
	if (nickname == "")
		nickname = "*";

	if (!argumentsArePresent_mod(cmd, 1, nickname))
	{
		//:server 409 <nick> :No origin specified
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 409 ") + nickname + " " + cmd.name + " :No origin specified\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return ;
	}

	std::string message;
	message = cmd.trailing;

	if(message == "" || message.size() < 1 || !cmd.params.empty())
	{
		//:server 409 <nick> :No origin specified
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 409 ") + nickname + " " + cmd.name + " :No origin specified\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return ;
	}
	
	std::string reply = "PONG :" + message + "\r\n";
	send(fd, reply.c_str(), reply.size(), 0);
}