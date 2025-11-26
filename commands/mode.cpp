
#include "irc.hpp"

void execMode(Command & cmd, int fd, std::vector<User> & uservect, std::vector<Channel> & channelvect)
{
	size_t i = searchVectWithFd(uservect, fd);

	std::string nickname = uservect[i].getNickName();
	if (nickname == "")
		nickname = "*";

	if (argumentsArePresent_mod(cmd, 1, nickname))
	{
		//461 <nick> MODE :Not enough parameters
		std::string reply = std::string(SERVER_NAME) + std::string(" 461 ") + nickname + " " + cmd.name + " :Not enough parameters\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
	}

	std::string channelname;
	if (!cmd.params.empty())
		channelname = cmd.params[0];
	else
		channelname = cmd.trailing;

	if (channelNameIsInvalid(cmd, fd, channelname, nickname))
		return ;
	
	if (argumentsArePresent_mod(cmd, 2, nickname))
	{
		//caso in cui c'e solo nome canale e si mostrano i permessi attuali
		//:server 324 <nick> #chat +ntkl password123 25
		return ;
	}

	//fare il ciclo principale che fa i vari + - i t k l o

}