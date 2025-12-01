
#include "irc.hpp"
#include <cerrno>

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

size_t searchVectWithNick(std::vector<User> & uservect, std::string & nickname)//da farla che ritorna index invece di un iterator che e piu sicuro
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

size_t searchChannel(std::vector<Channel> & channelvect, std::string & channelname)
{
	std::vector<Channel>::iterator it;
	size_t i = 0;
	for (it = channelvect.begin(); it != channelvect.end(); ++it)
	{
		if (it->getName() == channelname)
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

void deleteFromGroups(std::vector<Channel> & channelvect, int fd)
{
	if (channelvect.empty())//controllo potenzialmente superfluo ma non si sa mai
		return ;
	size_t sizee = channelvect.size();
	for (size_t i = 0; i < sizee; i++)
	{
		channelvect[i].removeFromUsers(fd);
		channelvect[i].removeFromOperators(fd);
		channelvect[i].removeFromInvites(fd);
		//se ultimo operatore esce dal canale (ma ci sono ancora utenti normali) canale rimane senza operatori perche irc e' cosi'
		if (channelvect[i].getUsersSize() == 0)
		{
			channelvect.erase(channelvect.begin() + i);
			--i;
			--sizee;
		}
	}
}

void safe_send(std::vector<User> & uservect, size_t i, std::string & toAdd)
{
	uservect[i].send_buffer += toAdd;
}

void trySendBuffer(std::vector<User> & uservect, std::vector<Channel> & channelvect, std::vector<pollfd> & fds, size_t & i)
{
	while (!uservect[i].send_buffer.empty())
	{
		int n = send(uservect[i].getFd(), uservect[i].send_buffer.c_str(), uservect[i].send_buffer.size(), 0);
		if (n > 0)
		{
			uservect[i].send_buffer.erase(0, n);  // remove bytes that were sent
		}
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			break; // cannot send more now, wait for next POLLOUT
		}
		else
		{
			size_t ind = searchVectWithFd(uservect, fds[i].fd);
			if (ind != uservect.size())
			{
				uservect.erase(uservect.begin() + ind);
				//levare utente anche da tutti i gruppi
				deleteFromGroups(channelvect, fds[i].fd);
			}
			close(fds[i].fd);
			std::cout << "Client disconnected (fd=" << fds[i].fd << ")\n";
			fds.erase(fds.begin() + i);
			--i;
		}
	}
}