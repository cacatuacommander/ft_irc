
#include "../irc.hpp"

void succesfulJoinMessages(Command & cmd, std::string & nickname, std::string & channelname, int fd, std::vector<Channel> & channelvect, size_t ch_i, std::vector<User> & uservect, size_t i)
{
	//:<nick>!<user>@<host> JOIN <channel>
	std::string message = ":" + nickname + "!" + uservect[i].getUserName() + "@" + uservect[i].getIp() + " " + cmd.name + " " + channelname + "\r\n";

	safe_send(uservect, fd, message);
	channelvect[ch_i].sendToAll(uservect, message, fd);

	//:<server> 332 <nick> <channel> :<topic>
	std::string msg;
	if (channelvect[ch_i].getTopic() == "No topic is set")
		msg = ":" + std::string(SERVER_NAME) + std::string(" 331 ") + nickname + " " + channelname + " :No topic is set\r\n";
	else
		msg = ":" + std::string(SERVER_NAME) + std::string(" 332 ") + nickname + " " + channelname + " :" + channelvect[ch_i].getTopic() + "\r\n";
	safe_send(uservect, fd, msg);
	//:<server> 353 <nick> = <channel> :<space-separated nicks>
	msg = ":" + std::string(SERVER_NAME) + std::string(" 353 ") + nickname + " = " + channelname + " :" + channelvect[ch_i].getListOfNicks(uservect) + "\r\n";
	safe_send(uservect, fd, msg);
	//:<server> 366 <nick> <channel> :End of /NAMES list.
	msg = ":" + std::string(SERVER_NAME) + std::string(" 366 ") + nickname + " " + channelname + " :End of /NAMES list.\r\n";
	safe_send(uservect, fd, msg);
}

bool channelAlredyExists(std::vector<Channel> & channelvect, std::string & newchannelname)
{
	if (channelvect.empty())
		return false;
	for (size_t i = 0; i < channelvect.size(); i++)
	{
		if (channelvect[i].getName() == newchannelname)
			return true;
	}
	return false;
}

void createChannel(Command & cmd, std::string & nickname, std::string & channelname, int fd, std::vector<Channel> & channelvect, size_t ch_i, std::vector<User> & uservect, size_t i)
{
	//controllare se ci sono troppi canali totali sul server e/o troppi canali per utente ? opzionale ma si puo fare
	if (channelAlredyExists(channelvect, channelname))//controllo inutile mi sa
		return ;
	channelvect.push_back(Channel(fd, channelname));
	succesfulJoinMessages(cmd, nickname, channelname, fd, channelvect, ch_i, uservect, i);
}

bool channelNameIsInvalid(Command & cmd, int fd, std::string & channelname, std::string & nickname, std::vector<User> & uservect)
{
	if (channelname == "")
	{
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 461 ") + nickname + " " + cmd.name + " :Not enough parameters\r\n";
		safe_send(uservect, fd, reply);
		return true;
	}
	else if ((channelname.size() < 2) || channelname.size() > 50 || channelname[0] != '#')//:server 476 nick ciao :Bad Channel Mask
	{
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 476 ") + nickname + " " + channelname + " :Bad Channel Mask\r\n";
		safe_send(uservect, fd, reply);
		return true;	
	}

	size_t i = 1;

	while (i < channelname.size())
	{
		if (!((channelname[i] >= 'A' && channelname[i] <= 'Z') || (channelname[i] >= '0' && channelname[i] <= '9') || (channelname[i] >= 'a' &&  channelname[i] <= 'z') || \
			channelname[i] == '_' || channelname[i] == '-'))
		{
			std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 476 ") + nickname + " " + channelname + " :Bad Channel Mask\r\n";
			safe_send(uservect, fd, reply);
			return true;
		}
		i++;
	}
	return false;
}

bool userAlredyPresent(int fd, std::vector<Channel> & channelvect, size_t ch_i)
{
	if (channelvect[ch_i].userIsInChannel(fd))
		return true;
	return false;
}

void addUserToChannel(Command & cmd, std::string & nickname, std::string & channelname, int fd, std::vector<Channel> & channelvect, size_t ch_i, std::vector<User> & uservect, size_t i)
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
			//:<server> 473 <nick> <channel> :Cannot join channel (+i)
			std::string msg = ":" + std::string(SERVER_NAME) + std::string(" 473 ") + nickname + " " + channelname + " :Cannot join channel (+i)\r\n";
			safe_send(uservect, fd, msg);
			return ;
		}
	}
	else if (channelvect[ch_i].needsPass())
	{
		if (!argumentsArePresent_mod(cmd, 2, nickname))//checks password exist
		{
			std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 475 ") + nickname + " " + channelname + " :Cannot join channel (+k)\r\n";
			safe_send(uservect, fd, reply);
			return ;
		}
		std::string password;
		if (cmd.params.size() >= 2)
			password = cmd.params[1];
		else
			password = cmd.trailing;

		if (password == "" || !channelvect[ch_i].checkPass(password))
		{
			std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 475 ") + nickname + " " + channelname + " :Cannot join channel (+k)\r\n";
			safe_send(uservect, fd, reply);
			return ;
		}
	}
	if (channelvect[ch_i].reachedUserLimit())
	{
			std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 471 ") + nickname + " " + channelname + " :Cannot join channel (+l)\r\n";
			safe_send(uservect, fd, reply);
			return ;
	}
	channelvect[ch_i].addToUsers(fd);
	if (nickname == "bot")
		channelvect[ch_i].addToOperators(fd);
	succesfulJoinMessages(cmd, nickname, channelname, fd, channelvect, ch_i, uservect, i);

}

void execJoin(Command & cmd, int fd, std::vector<User> & uservect, std::vector<Channel> & channelvect)
{
	size_t i = searchVectWithFd(uservect, fd);
	std::string nickname = uservect[i].getNickName();

	if (!argumentsArePresent(cmd, uservect, 1, nickname, fd))
		return ;

	std::string channelname;
	if (!cmd.params.empty())
		channelname = cmd.params[0];
	else
		channelname = cmd.trailing;

	if (channelNameIsInvalid(cmd, fd, channelname, nickname, uservect))
		return ;

	size_t ch_i = searchChannel(channelvect, channelname);

	if (ch_i  == channelvect.size())
	{
		createChannel(cmd, nickname, channelname, fd, channelvect, ch_i, uservect, i);//controllare anche e ci sono già troppi canali?
	}
	else
	{
		if (userAlredyPresent(fd, channelvect, ch_i))
			return ;//se utente è già presente nel canale, fa return senza mandare messaggi
		addUserToChannel(cmd, nickname, channelname, fd, channelvect, ch_i, uservect, i);
	}
}
