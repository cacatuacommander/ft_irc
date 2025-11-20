
#include "irc.hpp"

size_t searchVectWithFd(std::vector<User> & uservect, int fd)//da farla che ritorna index invece di un iterator che e piu sicuro
{
	std::vector<User>::iterator it;
	size_t i = 0;
	for (it = uservect.begin(); it != uservect.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			/* std::cerr << "fd: " << it->getFd() <<  std::endl;
			std::cerr << "nick: " << it->getNickName() <<  std::endl;
			std::cerr << "user: " << it->getUserName() <<  std::endl;
			std::cerr << "password: " << it->getPassword() <<  std::endl; */
			return i;
		}
		i++;
	}
	return i;
}

size_t searchVectWithNick(std::vector<User> & uservect, std::string nickname)//da farla che ritorna index invece di un iterator che e piu sicuro
{
	std::vector<User>::iterator it;
	size_t i = 0;
	for (it = uservect.begin(); it != uservect.end(); ++it)
	{
		if (it->getNickName() == nickname)
		{
			/* std::cerr << "fd: " << it->getFd() <<  std::endl;
			std::cerr << "nick: " << it->getNickName() <<  std::endl;
			std::cerr << "user: " << it->getUserName() <<  std::endl;
			std::cerr << "password: " << it->getPassword() <<  std::endl; */
			return i;
		}
		i++;
	}
	return i;
}