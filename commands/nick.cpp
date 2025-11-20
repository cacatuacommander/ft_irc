
#include "../irc.hpp"

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
	size_t i = searchVectWithNick(uservect, newnickname);
	if (i < uservect.size())
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 433 ") + oldnick + " " + newnickname + " :Nickname is already in use\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return true;
	}
	return false;
}

void execNick(Command cmd, int fd, std::vector<User> & uservect)
{
	size_t i = searchVectWithFd(uservect, fd);

	if (!argumentsArePresent(cmd, 1, uservect[i].getNickName(), fd))
		return ;

	std:: cout << "aaa" << std::endl;
	std::string newnickname;
	if(cmd.params.size() > 0)
		newnickname = cmd.params[1];
	else
		newnickname = cmd.trailing;
	
	if (i < uservect.size())
	{
		std::string oldnick = uservect[i].getNickName();
		if (oldnick == "")
			oldnick = "*";

		if (!isValidNickname(newnickname, fd, oldnick))
			return ;
		if (nicknameAlredyInUse(newnickname, fd, uservect, oldnick))
			return ;
		uservect[i].setNickName(newnickname);
		//std::cout << "nick aggiornato a:" << newnickname << std::endl;
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}
