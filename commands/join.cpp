
#include "irc.hpp"

bool channelNameIsInvalid(std::string & channelname, std::string nickname)
{
	if (channelname = "")
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 461 ") + nickname + " " + cmd.name + " :Not enough parameters\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return true;
	}
	else if (channelname[0] != "#" || (channelname.size() < 2) || channelname.size() > 50)//:server 476 nick ciao :Bad Channel Mask
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 476 ") + nickname + " " + channelname + " :Bad Channel Mask\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return true;	
	}
	int i = 0;
	while (i < channelname.size())
	{
		if (channelname[i] < 33 || channelname[i] = "," || channelname[i] = ":")
		{
			std::string reply = std::string(SERVER_NAME) + std::string(" 476 ") + nickname + " " + channelname + " :Bad Channel Mask\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
			return true;
		}
		i++;
	}
	return false;
}

bool userAlredyPresent(int fd, std::vector<Channel> & channelvect, size_t ch_i)
{
	if (channelvect[ch_i].userIsInGroup(fd))
		return true;
	return false;
}

bool addUserToChannel(Command & cmd, std::string & nickname, std::string & channelname, int fd, std::vector<Channel> & channelvect, size_t ch_i)
{
	if (channelvect[ch_i].needsInvite())
	{
		if (channelvect[ch_i].userIsInvited(fd))
		{
			channelvect[ch_i].removeFromInvites(fd);
/* 			channelvect[ch_i].addToUsers(fd); // questa parte da fare solo se si vuole che invito bypassi anche userlimit
			Inviare all’utente:la conferma di join
		//il topic del canale
		//la lista degli utenti (NAMES)
		//Inviare agli altri utenti del canale un messaggio di JOIN dell’utente entrato
			return ; */
		}
		else
		{
			error msg
		}
	}
	else if (channelvect[i].needsPass())
	{
		if (!argumentsArePresent(cmd, 2, nickname, fd))//checks password exist
		{//:irc.example.com 475 Alice #ciao :Cannot join channel (+k)
			std::string reply = std::string(SERVER_NAME) + std::string(" 475 ") + nickname + " " + channelname + " :Cannot join channel (+k)\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
			return ;
		}
		if (params.size() >= 2)
			std::string password = cmd.params[1];
		else
			std::string password = cmd.trailing;

		if (password == "" || |channelvect[i].checkPass(password))
		{
			std::string reply = std::string(SERVER_NAME) + std::string(" 475 ") + nickname + " " + channelname + " :Cannot join channel (+k)\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
			return ;
		}
	}
	if (channelvect[i].reachedUserLimit())
	{
		//:irc.example.com 471 Bob #general :Cannot join channel (+l)
			std::string reply = std::string(SERVER_NAME) + std::string(" 471 ") + nickname + " " + channelname + " :Cannot join channel (+l)\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
			return ;
	}
	check other stuff (?) mi sa non c'era altro da controllare a parte ban che non serve
	
	channelvect[ch_i].addToUsers(fd);
	Inviare all’utente:la conferma di join
	//il topic del canale
	//la lista degli utenti (NAMES)
	//Inviare agli altri utenti del canale un messaggio di JOIN dell’utente entrato
}

void execjoin(Command cmd, int fd, std::vector<User> & uservect, std::vector<Channel> & channelvect) 
{
	size_t i = searchVectWithFd(uservect, fd);
	std::string nickname = uservect[i].getNickName();

	if (!argumentsArePresent(cmd, 1, nickname, fd))
		return ;
		
	if (!params.empty())
		std::string channelname = cmd.params[0];
	else
		std::string channelname = cmd.trailing;

	if (channelNameIsInvalid(channelname, nickname))
		return ;

	size_t ch_i = searchChannel(channelvect, channelname);

	if (ch_i  == channelvect.size())
	{
		createChannel()//controllare anche e ci sono già troppi canali?
	}
	else
	{
		if (userAlredyPresent(fd, userchannel, ch_i))
			return ;//se utente è già presente nel canale, fa return senza mandare messaggi
		addUserToChannel(channelvect, i)
	}
}