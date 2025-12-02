
#include "user.hpp"

User::User(int newFd, std::string newip) : nickname(""), username (""), ip(newip), hasputpassword(false), fd(newFd), isverified(false), buffer("") {}

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

void User::setRealName(std::string & newrealname)
{
	this->realname = newrealname;
}

void User::setIsVerified()
{
	this->isverified = true;
}

void User::addToSendBuffer(std::string & toAdd)
{
	this->send_buffer += toAdd;
}

void User::sendBufferErase(int pos, int n)
{
	this->send_buffer.erase(pos, n);
}

void User::bufferAppend(char * buf, ssize_t n)
{
	this->buffer.append(buf, n);
}

std::string User::bufferSubstr(size_t pos, size_t n)
{
	return this->buffer.substr(pos, n);
}

bool User::sendBufferEmpty() const
{
	return this->send_buffer.empty();
}

void User::bufferErase(int pos, int n)
{
	this->buffer.erase(pos, n);
}

bool User::bufferEmpty() const
{
	return this->buffer.empty();
}

size_t User::bufferFind(const char * toFind) const
{
	return this->buffer.find(toFind);
}
	
std::string User::getBuffer() const
{
	return this->buffer;
}
const char * User::sendBufferCstr() const
{
	return this->send_buffer.c_str();
}

size_t User::sendBufferSize() const
{
	return this->send_buffer.size();
}

std::string User::getNickName() const
{
	return this->nickname;
}

std::string User::getUserName() const
{
	return this->username;
}

std::string User::getRealName() const
{
	return this->realname;
}

std::string User::getIp() const
{
	return this->ip;
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