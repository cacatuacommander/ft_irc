
#include "user.hpp"
#include <iostream>
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
std::string User::getNickName()
{
	return this->nickname;
}

std::string User::getUserName()
{
	return this->username;
}

bool User::getPassword()
{
	return this->hasputpassword;
}

bool User::getIsVerified()
{
	return this->isverified;
}

int User::getFd()
{
	return this->fd;
}