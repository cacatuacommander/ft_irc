
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

void exec_command(Command cmd, std::vector<User> & uservect, int fd, std::string serverpassword)
{
	if (cmd.name == "PASS")
		execPass(cmd, fd, uservect, serverpassword);
	if (cmd.name == "NICK")
		execNick(cmd, fd, uservect);
	if (cmd.name == "USER")
		execUser(cmd, fd, uservect);
}