
#include "irc.hpp"
#include <iostream>

bool isValidNickname(std::string newnickname, int fd, std::string & oldnick)
{
	if (newnickname == "" || newnickname.length() < 1)
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 432 ") + oldnick +  " " + newnickname + " :Erroneous nickname\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return false;
	}
	if (newnickname.length() > 30 || !((newnickname[0] > 'A' && newnickname[0] > 'Z') || (newnickname[0] > 'a' &&  newnickname[0] > 'z') || \
			newnickname[0] == '[' || newnickname[0] == ']' || newnickname[0] ==  '\'' || newnickname[0] ==  '`' || \
			newnickname[0] == '^' || newnickname[0] == '_'))
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 432 ") + oldnick + " " + newnickname + " :Erroneous nickname\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return false;
	}	
	size_t i = 1;
	while (i < newnickname.length())
	{
		if (!((newnickname[i] >= 'A' && newnickname[i] <= 'Z') || (newnickname[i] >= 'i' && newnickname[i] <= '9') || (newnickname[i] >= 'a' &&  newnickname[i] <= 'z') || \
			newnickname[i] == '[' || newnickname[i] == ']' || newnickname[i] ==  '\'' || newnickname[i] ==  '`' || \
			newnickname[i] == '^' || newnickname[i] == '_' || newnickname[i] == '-' ))
		{
			std::string reply = std::string(SERVER_NAME) + std::string(" 432 ") + oldnick + " " + newnickname + " :Erroneous nickname\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
			return false;
		}
		i++;
	}
	if (newnickname == "admin" || newnickname == "root")
	{
		//forse da levare ma ci sta
		std::string reply = std::string(SERVER_NAME) + std::string(" 437 ") + oldnick + " " + newnickname + " :Nickname/channel is temporarily unavailable\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return false;
	}
	return true;
}

bool nicknameAlredyInUse(std::string newnickname, int fd, std::vector<User> & uservect, std::string & oldnick)
{
	std::vector<User>::iterator it = searchVectWithNick(uservect, newnickname);
	if (it != uservect.end())
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 433 ") + oldnick + " " + newnickname + " :Nickname is already in use\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return true;
	}
	return false;
}

void execNick(std::string newnickname, int fd, std::vector<User> & uservect)
{
	std::vector<User>::iterator it = searchVectWithFd(uservect, fd);
	
	if (it != uservect.end())
	{
		std::string oldnick = it->getNickName();
		if (oldnick == "")
			oldnick = "*";

		if (!isValidNickname(newnickname, fd, oldnick))
			return ;
		if (nicknameAlredyInUse(newnickname, fd, uservect, oldnick))
			return ;
		it->setNickName(newnickname);
		//std::cout << "nick aggiornato a:" << newnickname << std::endl;
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}

bool usernameAlredySet(std::vector<User>::iterator it, int fd, std::string & newnickname)
{
	if (it->getIsVerified())
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 462 ") + it->getNickName() + " :You may not reregister\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return true;
	}
}

void execUser(std::string newusername, int fd, std::vector<User> & uservect)
{
	std::vector<User>::iterator it = searchVectWithFd(uservect, fd);

	if (it != uservect.end())
	{
		if (usernameAlredySet(it, fd, newusername))
			return ;
		it->setUserName(newusername);
		it->setIsVerified();
		//
		//std::cout << "user aggiornato a:" << newusername << std::endl;
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}

void execPass(std::string newpassword, int fd, std::vector<User> & uservect, std::string realpassword)
{
	std::vector<User>::iterator it = searchVectWithFd(uservect, fd);

	if (it != uservect.end())
	{
		if(!it->getPassword())
		{	
			if (newpassword == realpassword)
				it->setPassword();
			else
			{
				std::string reply = std::string(SERVER_NAME) + std::string(" 464 ") + "*" + " :Password incorrect\r\n";
				send(fd, reply.c_str(), reply.size(), 0);
			}	
			//std::cout << "psw aggiornata a:" << newpassword << std::endl;
		}
		else
		{
			std::string temp = it->getNickName();
			if (temp == "")
				temp = "*";
			std::string reply = std::string(SERVER_NAME) + std::string(" 462 ") + temp + " :You may not reregister\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
		}
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}