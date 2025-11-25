#include "channel.hpp"

Channel::Channel(std::string newuser, std::string channelname) : name(channelname), topic(""), isinviteonly(false), onlyopsmaysettopic(false), password(""), userlimit(-1) {}

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

bool Channel::needsInvite() const;
{
	return this->isinviteonly;
}

bool Channel::needsPass() const
{
	return (this->password == "");
}

bool Channel::userIsInGroup(int fd) const
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