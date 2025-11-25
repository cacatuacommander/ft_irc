#include "channel.hpp"
#include "irc.hpp"
#include <arpa/inet.h>
#include <algorithm>

Channel::Channel(int fd, std::string channelname) : name(channelname), topic(""), isinviteonly(false), password(""), userlimit(-1)
{
	users.push_back(fd);
	operators.push_back(fd);
}

Channel::~Channel() {}

void Channel::addToUsers(int fd)
{
	this->users.push_back(fd);
}

void Channel::removeFromInvites(int fd)
{
	if (this->invites.empty())//controllo che forse non dovrebbe servire
		return ;
	for (size_t i = 0; i < invites.size(); i++)
	{
		if (invites[i] == fd)
		{
			invites.erase(invites.begin() + i);
			return ;
		}
	}
}

std::string Channel::getName() const
{
	return this->name;
}

std::string Channel::getTopic() const
{
	if (this->topic.empty() || topic == "")
		return "No topic is set";
	return this->topic;
}

bool Channel::needsInvite() const
{
	return this->isinviteonly;
}

bool Channel::needsPass() const
{
	return (this->password == "");
}

bool Channel::userIsInChannel(int fd) const
{
	if (this->users.empty())
		return false;
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i] == fd)
			return true;
	}
	return false;
}

bool Channel::userIsInvited(int fd) const
{
	if (this->invites.empty())
		return false;
	for (size_t i = 0; i < invites.size(); i++)
	{
		if (invites[i] == fd)
			return true;
	}
	return false;
}

bool Channel::reachedUserLimit() const
{
	if (this->userlimit < 0)
		return false;
	if (this->users.size() >= this->userlimit)
		return true;
	return false;
}

bool Channel::checkPass(std::string & passwordtocheck) const
{
	if (passwordtocheck.empty() || passwordtocheck == "")
		return false;
	if (this->password == passwordtocheck)
		return true;
	return false;
}

void Channel::sendToAll(std::string message)
{
	if (this->users.empty())//controllo che forse non serve
		return ;
	for (size_t i = 0; i < users.size(); i++)
	{
		send(this->users[i], message.c_str(), message.size(), 0);
	}
}

std::string Channel::getListOfNicks(std::vector<Channel> & channelvect, std::vector<User> & uservect) const
{
	std::string listofnames = "";
	if (users.empty())
		return "";
	for (size_t i = 0; i < users.size(); i++)
	{
		if (i != 0)
		listofnames += " ";
		if (std::find(this->operators.begin(), this->operators.end(), this->users[i]) != this->operators.end());
			listofnames += "@";
		listofnames += uservect[searchVectWithFd(uservect, this->users[i])].getNickName();
	}
	return listofnames;
}