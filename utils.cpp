
#include "irc.hpp"

std::vector<User>::iterator searchVectWithFd(std::vector<User> uservect, int fd)
{
	std::vector<User>::iterator it;
	for (it = uservect.begin(); it != uservect.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			/* std::cerr << "fd: " << it->getFd() <<  std::endl;
			std::cerr << "nick: " << it->getNickName() <<  std::endl;
			std::cerr << "user: " << it->getUserName() <<  std::endl;
			std::cerr << "password: " << it->getPassword() <<  std::endl; */
			return it;
		}
	}
	return it;
}

std::vector<User>::iterator searchVectWithNick(std::vector<User> uservect, std::string nickname)
{
	std::vector<User>::iterator it;
	for (it = uservect.begin(); it != uservect.end(); ++it)
	{
		if (it->getNickName() == nickname)
		{
			/* std::cerr << "fd: " << it->getFd() <<  std::endl;
			std::cerr << "nick: " << it->getNickName() <<  std::endl;
			std::cerr << "user: " << it->getUserName() <<  std::endl;
			std::cerr << "password: " << it->getPassword() <<  std::endl; */
			return it;
		}
	}
	return it;
}