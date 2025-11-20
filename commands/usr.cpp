#include "../irc.hpp"

bool isValidusername(std::string newusername, int fd, std::string oldnick) //:server 468 <nick> :Invalid username
{
	if (newusername == "" || newusername.length() < 1)
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 468 ") + oldnick + " :Invalid username\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return false;
	}
	if (newusername.length() > 9 || !((newusername[0] > 'A' && newusername[0] > 'Z') || (newusername[0] > 'a' &&  newusername[0] > 'z') || \
			newusername[0] == '-' || newusername[0] == '_'))
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 468 ") + oldnick + " :Invalid username\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return false;
	}	
	size_t i = 1;
	while (i < newusername.length())
	{
		if (!((newusername[i] >= 'A' && newusername[i] <= 'Z') || (newusername[i] >= 'i' && newusername[i] <= '9') || (newusername[i] >= 'a' &&  newusername[i] <= 'z') || \
			newusername[i] == '_' || newusername[i] == '-' ))
		{
			std::string reply = std::string(SERVER_NAME) + std::string(" 468 ") + oldnick + " :Invalid username\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
			return false;
		}
		i++;
	}
	return true;
}

bool usernameAlredySet(int i, std::vector<User> & uservect, int fd)
{
	if (uservect[i].getIsVerified())
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 462 ") + uservect[i].getNickName() + " :You may not reregister\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return true;
	}
	return false;
}

void execUser(Command cmd, int fd, std::vector<User> & uservect)
{
	size_t i = searchVectWithFd(uservect, fd);

	if (usernameAlredySet(i, uservect, fd))
		return ;
	if (!argumentsArePresent(cmd, 4, uservect[i].getNickName(), fd))
		return ;

	std::string newusername = cmd.params[0];
	std::string realname;
	if(cmd.params.size() >= 4)
		realname = cmd.params[3];
	else
		realname = cmd.trailing;

	if (i < uservect.size())
	{
		if (!isValidusername(newusername, fd, uservect[i].getNickName()))
			return ;
		uservect[i].setUserName(newusername);
		uservect[i].setRealName(realname);
		uservect[i].setIsVerified();
		//fare tutti i send di benvenuto:
		//welcome_messages();
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}