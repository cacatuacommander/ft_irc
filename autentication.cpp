
#include "irc.hpp"
#include <iostream>

bool argumentsArePresent(Command cmd, int numbofargneeded)
{
	unsigned int argsn = cmd.params.size();
	if (!cmd.trailing.empty())
		argsn++;
	if (argsn >= numbofargneeded)
		return true;
	return false;
}

void exec_command(Command cmd, std::vector<User> uservect, int fd, std::string serverpassword)
{
	if (cmd.name == "PASS")
		execPass(cmd, fd, uservect, serverpassword);
	if (cmd.name == "NICK")
		execNick(cmd, fd, uservect);
	if (cmd.name == "USER")
		execUser(cmd, fd, uservect);
}