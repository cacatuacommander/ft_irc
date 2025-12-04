#include "../irc.hpp"

bool checkParamsInvite(const Command &cmd, const std::string &nick, int fd, std::vector<User> &uservect, std::vector<Channel> &channelVect)
{
	if (cmd.params.size() < 2)
	{
		std::string msg = ":" + std::string(SERVER_NAME) + " 461 " + nick +
						  " INVITE :Not enough parameters\r\n";
		safe_send(uservect, fd, msg);
		return false;
	}

	std::string targetNick = cmd.params[0];
	std::string channelName = cmd.params[1];
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
						  " " + channelName + 
						  " " + targetNick + " :No such nick\r\n";
		safe_send(uservect, fd, msg);
		return false;
	}

	if (channel.userIsInChannel(uservect[targetIdx].getFd()))
	{
		std::string msg = ":" + std::string(SERVER_NAME) + " 443 " + nick +
						  " " + channelName + " " + targetNick + " :is already on channel\r\n";
		safe_send(uservect, fd, msg);
		return false;
	}
	return true;
}

void execInvite(Command cmd, int fd, std::vector<Channel>& channelVect, std::vector<User> & uservect)
{
	size_t sender_index = searchVectWithFd(uservect, fd);
	std::string nick = uservect[sender_index].getNickName().empty() ? "*" : uservect[sender_index].getNickName();
	if (!checkParamsInvite(cmd, nick, fd, uservect, channelVect))
		return ;
	size_t channel_index = searchChannel(channelVect, cmd.params[1]);
	size_t target_index = searchVectWithNick(uservect, cmd.params[0]);
	Channel &channel = channelVect[channel_index];
	User &sender = uservect[sender_index];
	User &target = uservect[target_index];

	if (!channel.userIsInvited(target.getFd()))
		channel.addUserToInviteList(target.getFd());
	std::string msg = ":" + std::string(SERVER_NAME) + " 341 " +
				sender.getNickName() + " " +
				target.getNickName() + " " +
				channel.getName() + "\r\n";

	safe_send(uservect, fd, msg);
	msg = ":" + sender.getNickName() + " INVITE " +
			target.getNickName() + " " +
			channel.getName() + "\r\n";
	safe_send(uservect, target.getFd(), msg);
}
