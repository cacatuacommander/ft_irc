
#include "irc.hpp"
#include <cerrno>

size_t searchVectWithFd(std::vector<User> & uservect, int fd)
{
	std::vector<User>::iterator it;
	size_t i = 0;
	for (it = uservect.begin(); it != uservect.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			return i;
		}
		i++;
	}
	return i;
}

size_t searchVectWithNick(std::vector<User> & uservect, std::string & nickname)
{
	std::vector<User>::iterator it;
	size_t i = 0;
	for (it = uservect.begin(); it != uservect.end(); ++it)
	{
		if (it->getNickName() == nickname)
		{
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
			return i;
		}
		i++;
	}
	return i;
}

void deleteFromGroups(std::vector<Channel> & channelvect, int fd, std::vector<User> & uservect, std::string msg)
{
	if (channelvect.empty())//controllo potenzialmente superfluo ma non si sa mai
		return ;
	size_t sizee = channelvect.size();
	for (size_t i = 0; i < sizee; i++)
	{
		channelvect[i].sendToAll(uservect, msg, fd);
		channelvect[i].removeFromUsers(fd);
		channelvect[i].removeFromOperators(fd);
		channelvect[i].removeFromInvites(fd);
		//se ultimo operatore esce dal canale (ma ci sono ancora utenti normali) canale rimane senza operatori perche irc e' cosi'
		if (channelvect[i].getUsersSize() == 0 || (channelvect[i].getUsersSize() == 1 && (channelvect[i].getListOfNicks(uservect) == "@bot" || channelvect[i].getListOfNicks(uservect) == "bot")))
		{
			channelvect.erase(channelvect.begin() + i);
			--i;
			--sizee;
		}
	}
}

void safe_send(std::vector<User> & uservect, int fd, std::string & toAdd)
{
	size_t i = searchVectWithFd(uservect, fd);
	if (i < uservect.size())
		uservect[i].addToSendBuffer(toAdd);
/* 	int n = send(fd, toAdd.c_str(), toAdd.size(), 0);
	if (n > 0)
	{
		if (i < uservect.size() && static_cast<size_t>(n) <= uservect[i].sendBufferSize())
			uservect[i].sendBufferErase(0, n);
	} */
}

void trySendBuffer(std::vector<User> & uservect, std::vector<Channel> & channelvect, std::vector<pollfd> & fds, size_t & i)
{
	while (i < uservect.size() && !uservect[i].sendBufferEmpty())
	{
		int n = send(uservect[i].getFd(), uservect[i].sendBufferCstr(), uservect[i].sendBufferSize(), 0);
		std::cerr << "n: " << n << " send_buffer: '" << uservect[i].sendBufferCstr() << "'" << std::endl;
		if (n > 0)
		{
			if (i < uservect.size() && static_cast<size_t>(n) <= uservect[i].sendBufferSize())
			{
				std::cerr << " sei quiii111" << std::endl;
				uservect[i].sendBufferErase(0, n);  // remove bytes that were sent
			}
		}
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			break; // cannot send more now, wait for next POLLOUT
		}
		else
		{
			std::cerr << " sei quaa " << std::endl;
			/* (void) channelvect;
			(void) fds; */
			size_t ind = searchVectWithFd(uservect, fds[i].fd);
			if (ind != uservect.size())
			{
				uservect.erase(uservect.begin() + ind);
				std::string msg = ":" + uservect[ind].getNickName() + " QUIT : Client exited\r\n";
				deleteFromGroups(channelvect, fds[i].fd, uservect, msg);
			}
			close(fds[i].fd);
			std::cout << "Client disconnected (fd=" << fds[i].fd << ")\n";
			fds.erase(fds.begin() + i);
			--i;
		} 
	}
}