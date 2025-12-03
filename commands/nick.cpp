
#include "../irc.hpp"

bool isValidNickname(std::string newnickname, int fd, std::string & oldnick, std::vector<User> & uservect)
{
	if (newnickname == "" || newnickname.length() < 1)
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 432 ") + oldnick +  " " + newnickname + " :Erroneous nickname\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	if (newnickname.length() > 30 || !((newnickname[0] >= 'A' && newnickname[0] <= 'Z') || (newnickname[0] >= 'a' &&  newnickname[0] <= 'z') || \
			newnickname[0] == '[' || newnickname[0] == ']' || newnickname[0] ==  '\'' || newnickname[0] ==  '`' || \
			newnickname[0] == '^' || newnickname[0] == '_'))
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 432 ") + oldnick + " " + newnickname + " :Erroneous nickname\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}	
	size_t i = 1;
	while (i < newnickname.length())
	{
		if (!((newnickname[i] >= 'A' && newnickname[i] <= 'Z') || (newnickname[i] >= '0' && newnickname[i] <= '9') || (newnickname[i] >= 'a' &&  newnickname[i] <= 'z') || \
			newnickname[i] == '[' || newnickname[i] == ']' || newnickname[i] ==  '\'' || newnickname[i] ==  '`' || \
			newnickname[i] == '^' || newnickname[i] == '_' || newnickname[i] == '-' ))
		{
			std::string reply = std::string(SERVER_NAME) + std::string(" 432 ") + oldnick + " " + newnickname + " :Erroneous nickname\r\n";
			safe_send(uservect, fd, reply);
			return false;
		}
		i++;
	}
	if (newnickname == "admin" || newnickname == "root" || (newnickname == "bot" && uservect.size() > 1))
	{
		//forse da levare ma ci sta
		std::string reply = std::string(SERVER_NAME) + std::string(" 437 ") + oldnick + " " + newnickname + " :Nickname/channel is temporarily unavailable\r\n";
		safe_send(uservect, fd, reply);
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
		safe_send(uservect, fd, reply);
		return true;
	}
	return false;
}

void execNick(Command cmd, int fd, std::vector<User> & uservect, std::vector<Channel> & channelvect)
{
	size_t i = searchVectWithFd(uservect, fd);

	if (i < uservect.size())
	{
		std::string oldnick = uservect[i].getNickName();
		if (oldnick == "")
			oldnick = "*";

		if (!argumentsArePresent_mod(cmd, 1, oldnick))
		{
			//:<server> 431 <nick> :No nickname given
			std::string reply = std::string(SERVER_NAME) + std::string(" 431 ") + oldnick + " :No nickname given\r\n";
			safe_send(uservect, fd, reply);
			return ;
		}
		std::string newnickname;
		if(cmd.params.size() > 0)
			newnickname = cmd.params[0];
		else
			newnickname = cmd.trailing;

		if (!isValidNickname(newnickname, fd, oldnick, uservect))
			return ;
		if (nicknameAlredyInUse(newnickname, fd, uservect, oldnick))
			return ;

		std::string msg = ":" + uservect[i].getNickName() + " NICK :" + newnickname + "\r\n";
		for (size_t n = 0; n < channelvect.size(); ++n)
		{
			if (channelvect[n].userIsInChannel(fd))
			{
				channelvect[n].sendToAll(uservect, msg, fd);
			}
		}
		uservect[i].setNickName(newnickname);
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}
