
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
		if (channelvect[i].userIsInChannel(fd))
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

void trySendBuffer(std::vector<User> & uservect, std::vector<Channel> & channelvect, std::vector<pollfd> & fds, size_t & fd_i, size_t us_i)
{
	while (us_i < uservect.size() && !uservect[us_i].sendBufferEmpty())
	{
		int n = send(uservect[us_i].getFd(), uservect[us_i].sendBufferCstr(), uservect[us_i].sendBufferSize(), MSG_NOSIGNAL);
		//std::cerr << "n: " << n << " send_buffer: '" << uservect[us_i].sendBufferCstr() << "'" << std::endl;
		if (n > 0)
		{
			if (us_i < uservect.size() && static_cast<size_t>(n) <= uservect[us_i].sendBufferSize())
			{
				//std::cerr << " sei quiii111" << std::endl;
				uservect[us_i].sendBufferErase(0, n);  // remove bytes that were sent
			}
		}
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			break; // cannot send more now, wait for next POLLOUT
		}
		else
		{
			//std::cerr << " sei quaa " << std::endl;
			/* (void) channelvect;
			(void) fds; */
			size_t ind = searchVectWithFd(uservect, fds[fd_i].fd);
			if (ind != uservect.size())
			{
				std::string msg = ":" + uservect[ind].getNickName() + " QUIT : Client exited\r\n";
				uservect.erase(uservect.begin() + ind);
				deleteFromGroups(channelvect, fds[fd_i].fd, uservect, msg);
			}
			close(fds[fd_i].fd);
			std::cout << "Client disconnected (fd=" << fds[fd_i].fd << ")\n";
			fds.erase(fds.begin() + fd_i);
			--fd_i;
		} 
	}
}