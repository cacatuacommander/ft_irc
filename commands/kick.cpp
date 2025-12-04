#include "../irc.hpp"

bool checkParamsKick(const Command &cmd, const std::string &nick, int fd, std::vector<User> &uservect, std::vector<Channel> &channelVect)
{
	if (cmd.params.size() < 2)
	{
		std::string msg = ":" + std::string(SERVER_NAME) + " 461 " + nick +
						  " KICK :Not enough parameters\r\n";
		safe_send(uservect, fd, msg);
		return false;
	}

	std::string channelName = cmd.params[0];
	std::string targetNick = cmd.params[1];
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

	if (!channel.searchFDinOperators(fd))
	{
		std::string msg = ":" + std::string(SERVER_NAME) + " 482 " + nick +
						  " " + channelName + " :You're not channel operator\r\n";
		safe_send(uservect, fd, msg);
		return false;
	}

	size_t targetIdx = searchVectWithNick(uservect, targetNick);
	if (targetIdx == uservect.size())
	{
		std::string msg = ":" + std::string(SERVER_NAME) + " 401 " + nick +
						  " " + targetNick + " :No such nick\r\n";
		safe_send(uservect, fd, msg);
		return false;
	}

	if (!channel.userIsInChannel(uservect[targetIdx].getFd()))
	{
		std::string msg = ":" + std::string(SERVER_NAME) + " 441 " + nick +
						  " " + targetNick + " " + channelName +
						  " :They aren't on that channel\r\n";
		safe_send(uservect, fd, msg);
		return false;
	}

	return true;
}

void execKick(Command cmd, int fd, std::vector<Channel>& channelVect, std::vector<User> & uservect)
{
	size_t sender_index = searchVectWithFd(uservect, fd);
	std::string nick = uservect[sender_index].getNickName().empty() ? "*" : uservect[sender_index].getNickName();
	if (!checkParamsKick(cmd, nick, fd, uservect, channelVect))
		return ;
	size_t channel_index = searchChannel(channelVect, cmd.params[0]);
	size_t target_index = searchVectWithNick(uservect, cmd.params[1]);
	Channel &channel = channelVect[channel_index];
	User &sender = uservect[sender_index];
	User &target = uservect[target_index];

	std::string kick_reason = cmd.trailing.empty() ? sender.getNickName() : cmd.trailing;
	std::string msg = ":" + sender.getNickName() + "!" +
				sender.getUserName() + "@" +
				sender.getIp() + " KICK " +
				channel.getName() + " " +
				target.getNickName() + " :" + kick_reason + "\r\n";

	channel.sendToAll(uservect, msg, fd);
	channel.removeUser(target.getFd());
}
