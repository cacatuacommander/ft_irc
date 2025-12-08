
#include "irc.hpp"
#include <sys/poll.h>
#include <vector>

bool argumentsArePresent(Command cmd, std::vector<User> & uservect, unsigned int numbofargneeded, std::string nickname, int fd)
{
	if (nickname == "")
		nickname = "*";

	unsigned int argsn = cmd.params.size();
	if (!cmd.trailing.empty())
		argsn++;
	if (argsn >= numbofargneeded)
		return true;

	std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 461 ") + nickname + " " + cmd.name + " :Not enough parameters\r\n";
	safe_send(uservect, fd, reply);

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

void exec_command(Command cmd, std::vector<User> & uservect, std::vector<Channel> & channelvect, std::string serverpassword, std::vector<pollfd> &fds, int i)
{
	if (cmd.name == "PASS")
		execPass(cmd, fds[i].fd, uservect, serverpassword);
	if (cmd.name == "NICK")
		execNick(cmd, fds[i].fd, uservect, channelvect);
	if (cmd.name == "USER")
		execUser(cmd, fds[i].fd, uservect);
	if (cmd.name == "JOIN")
		execJoin(cmd, fds[i].fd, uservect, channelvect);
	if (cmd.name == "PRIVMSG")
		execPrivMsg(cmd, fds[i].fd, channelvect, uservect);
	if (cmd.name == "KICK")
		execKick(cmd, fds[i].fd, channelvect, uservect);
	if (cmd.name == "INVITE")
		execInvite(cmd, fds[i].fd, channelvect, uservect);
	if (cmd.name == "TOPIC")
		execTopic(cmd, fds[i].fd, channelvect, uservect);
	if (cmd.name == "MODE")
		execMode(cmd, fds[i].fd, uservect, channelvect);
	if (cmd.name == "PART")
		execPart(cmd, fds[i].fd, uservect, channelvect);
	if (cmd.name == "QUIT")
		execQuit(cmd, fds[i].fd, uservect, channelvect, fds, i);
	if (cmd.name == "PING")
		execPing(cmd, fds[i].fd, uservect);
	if (cmd.name == "BOT")
		execBotCall(cmd, fds[i].fd, channelvect, uservect);
}
