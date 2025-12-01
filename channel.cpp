#include "channel.hpp"
#include "user.hpp"
#include <arpa/inet.h>
#include <algorithm>

size_t searchVectWithFd(std::vector<User> & uservect, int fd);

Channel::Channel(int fd, std::string channelname) : name(channelname), topic(""), istopicrestricted(false), isinviteonly(false), password(""), userlimit(-1)

{
	users.push_back(fd);
	operators.push_back(fd);
}

Channel::~Channel() {}

void Channel::setIsTopicRestricted(bool newistopicrestricted)
{
	this->istopicrestricted = newistopicrestricted;
}

void Channel::setIsInviteOnly(bool newisinviteonly)
{
	this->isinviteonly = newisinviteonly;
}

void Channel::setPass(std::string newpassword)
{
	this->password = newpassword;
}

void Channel::setUserLimit(int newuserlimit)
{
	this->userlimit = newuserlimit;
}

void Channel::addToUsers(int fd)
{
	this->users.push_back(fd);
}

void Channel::addToOperators(int fd)
{
	this->operators.push_back(fd);
}

void Channel::removeFromUsers(int fd)
{
	if (this->users.empty())//controllo che forse non dovrebbe servire
		return ;
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i] == fd)
		{
			users.erase(users.begin() + i);
			return ;
		}
	}
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

void Channel::removeFromOperators(int fd)
{
	if (this->operators.empty())//controllo che forse non dovrebbe servire
		return ;
	for (size_t i = 0; i < operators.size(); i++)
	{
		if (operators[i] == fd)
		{
			operators.erase(operators.begin() + i);
			return ;
		}
	}
}

std::string Channel::getName() const
{
	return this->name;
}


std::string Channel::getPass() const
{
	return this->password;
}


std::string Channel::getTopic() const
{
	if (this->topic.empty() || topic == "")
		return "No topic is set";
	return this->topic;
}


int Channel::getUserlimit() const
{
	return this->userlimit;
}


bool Channel::needsInvite() const
{
	return this->isinviteonly;
}

bool Channel::needsPass() const
{
	if (this->password.empty() || this->password == "")
		return false;
	return true;
}

bool Channel::isTopicResticted() const
{
	return this->istopicrestricted;
}

bool Channel::userIsInOperators(int fd) const
{
	if (this->operators.empty())
		return false;
	for (size_t i = 0; i < operators.size(); i++)
	{
		if (operators[i] == fd)
			return true;
	}
	return false;
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
	if (this->users.size() >= static_cast<size_t>(this->userlimit))
		return true;
	return false;
}

size_t Channel::getOperatorsSize() const
{
	return this->operators.size(); 
}

size_t Channel::getUsersSize() const
{
	return this->users.size(); 
}

bool Channel::checkPass(std::string & passwordtocheck) const
{
	if (passwordtocheck.empty() || passwordtocheck == "")
		return false;
	if (this->password == passwordtocheck)
		return true;
	return false;
}

void Channel::sendToAll(std::string message, int fd)
{
	if (this->users.empty())//controllo che forse non serve
		return ;
	for (size_t i = 0; i < users.size(); i++)
	{
		if (fd != this->users[i])
			send(this->users[i], message.c_str(), message.size(), 0);
	}
}

bool Channel::removeUser(int fd)
{
	std::vector<int>::iterator it;
	it = std::find(operators.begin(), operators.end(), fd);
	if (it != operators.end())
	{
		operators.erase(it);
	}
	
	it = std::find(users.begin(), users.end(), fd);
	if (it != users.end())
	{
		users.erase(it);
		return true;
	}

	return false;
}

void Channel::changeTopic(std::string new_topic)
{
	this->topic = new_topic;
}


void Channel::addUserToInviteList(int fd) 
{
	invites.push_back(fd);
}

bool Channel::searchFDinOperators(int fd)
{
	if (std::find(this->operators.begin(), this->operators.end(), fd) != this->operators.end())
		return true;
	return false;
}

std::string Channel::getListOfNicks(std::vector<User> & uservect) const
{
	std::string listofnames = "";
	if (users.empty())
		return "";
	for (size_t i = 0; i < users.size(); i++)
	{
		if (i != 0)
			listofnames += " ";

		if (std::find(this->operators.begin(), this->operators.end(), this->users[i]) != this->operators.end())
			listofnames += "@";
		listofnames += uservect[searchVectWithFd(uservect, this->users[i])].getNickName();
	}
	return listofnames;
}
