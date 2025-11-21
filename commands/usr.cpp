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
void send_welcome_messages(int i, std::vector<User> & uservect, int fd)
{
	std::string nick = uservect[i].getNickName();//:server 001 <nick> :Welcome to the IRC Network, <nick>!<user>@<host>
	std::string message = std::string(SERVER_NAME) + std::string(" 001 ") + nick + " :Welcome to the IRC Network, " + nick + "!" + uservect[i].getNickName() + "@" + uservect[i].getIp() + "\r\n";
	send(fd, message.c_str(), message.size(), 0);//:server 002 <nick> :Your host is <servername>, running version <version>
	
	message = std::string(SERVER_NAME) + std::string(" 002 ") + nick + " :Your host is " + "our.server.irc" + ", running version "+ std::string(SERVER_VERSION) + "\r\n";
	send(fd, message.c_str(), message.size(), 0);
	
	message = std::string(SERVER_NAME) + std::string(" 003 ") + nick + " :This server was created " + std::string(SERVER_CREATION_DATE) + "\r\n";
	send(fd, message.c_str(), message.size(), 0);//:server 003 <nick> :This server was created <date>//:server 004 <nick> <servername> <version> <usermodes> <chanmodes>
	
	message = std::string(SERVER_NAME) + std::string(" 004 ") + nick + " :This server was created " + std::string(SERVER_NAME) + " " + std::string(SERVER_VERSION) + " iwso itkol" +"\r\n";
	send(fd, message.c_str(), message.size(), 0);//:server 005 <nick> <parameter1> <parameter2> ... :<optional comment>
	
	message = std::string(SERVER_NAME) + std::string(" 005 ") + nick + " :CHANTYPES=# PREFIX=(o)@ CHANMODES=k,l,imnt NETWORK=myircc :are supported by this server\r\n";
	send(fd, message.c_str(), message.size(), 0);
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

		send_welcome_messages(i, uservect, fd);
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}