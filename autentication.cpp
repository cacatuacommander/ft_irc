
#include "irc.hpp"

bool argumentsArePresent(Command cmd, unsigned int numbofargneeded, std::string nickname, int fd)
{
	if (nickname == "")
		nickname = "*";

	unsigned int argsn = cmd.params.size();
	if (!cmd.trailing.empty())
		argsn++;
	if (argsn >= numbofargneeded)
		return true;

	std::string reply = std::string(SERVER_NAME) + std::string(" 461 ") + nickname + " " + cmd.name + " :Not enough parameters\r\n";
	send(fd, reply.c_str(), reply.size(), 0);

	return false;
}

bool argumentsArePresent_mod(Command cmd, unsigned int numbofargneeded, std::string nickname)
{
	if (nickname == "")
		nickname = "*";

	size_t argsn = cmd.params.size();
	if (!cmd.trailing.empty())
		argsn++;
	if (argsn >= numbofargneeded)
		return true;

	return false;
}

void exec_command(Command cmd, std::vector<User> & uservect, std::vector<Channel> & channelvect, int fd, std::string serverpassword)
{
	if (cmd.name == "PASS")
		execPass(cmd, fd, uservect, serverpassword);
	if (cmd.name == "NICK")
		execNick(cmd, fd, uservect);
	if (cmd.name == "USER")
		execUser(cmd, fd, uservect);
	if (cmd.name == "JOIN")
		execJoin(cmd, fd, uservect, channelvect);
	if (cmd.name == "PRIVMSG")
		execPrivMsg(cmd, fd, channelvect, uservect);
	if (cmd.name == "KICK")
		execKick(cmd, fd, channelvect, uservect);
	if (cmd.name == "INVITE")
		execInvite(cmd, fd, channelvect, uservect);
	if (cmd.name == "TOPIC")
		execTopic(cmd, fd, channelvect, uservect);
	if (cmd.name == "MODE")
		execMode(cmd, fd, uservect, channelvect);
	if (cmd.name == "PING")
		execPing(cmd, fd, uservect);
}
