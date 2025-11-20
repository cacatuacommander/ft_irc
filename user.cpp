
#include "user.hpp"

User::User(int newFd) : nickname(""), username (""), hasputpassword(false), fd(newFd), isverified(false) {}

User::~User() {}

void User::setPassword()
{
	this->hasputpassword = true;
}

void User::setNickName(std::string & newnickname)
{
	this->nickname = newnickname;
}

void User::setUserName(std::string & newusername)
{
	this->username = newusername;
}

void User::setIsVerified()
{
	this->isverified = true;
}

std::string User::getNickName() const
{
	return this->nickname;
}

std::string User::getUserName() const
{
	return this->username;
}

bool User::getPassword() const
{
	return this->hasputpassword;
}

bool User::getIsVerified() const
{
	return this->isverified;
}

int User::getFd() const
{
	return this->fd;
}