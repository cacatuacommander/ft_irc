
#include "../irc.hpp"
#include <sstream>
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <limits>

void getCurrentChannelStatus(std::string & nickname, std::string & channelname, size_t ch_i, std::vector<Channel> & channelvect, int fd, std::vector<User> & uservect)
{
	std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 324 ") + nickname + " " + channelname + " +";
	if (channelvect[ch_i].needsInvite())
		reply += "i";
	if (channelvect[ch_i].isTopicResticted())
		reply += "t";
	if (channelvect[ch_i].needsPass())
		reply += "k";
	if (channelvect[ch_i].getUserlimit() > 0)
		reply += "l";
	if (channelvect[ch_i].needsPass())
		reply = reply + " " + channelvect[ch_i].getPass();
	if (channelvect[ch_i].isTopicResticted())
		reply = reply + " " + channelvect[ch_i].getTopic();
	if (channelvect[ch_i].getUserlimit() > 0)
	{
		std::ostringstream oss;
		oss << channelvect[ch_i].getUserlimit();
		reply += " " + oss.str();
	}
	reply += "\r\n";
	safe_send(uservect, fd, reply);
}

bool ChannelPassIsValid(std::string newpass)
{
	if (newpass.size() < 1 || newpass.size() > 50)
		return false;
	size_t i = 0;
	while(newpass[i])
	{
		if (!(newpass[i] > 32 && newpass[i] != ',' && newpass[i] != ':' && isprint(newpass[i]))) 
			return false;
		i++;
	}
	return true;
}

bool addNewPassToChannel(Command & cmd, int fd, std::vector<Channel> & channelvect, size_t & ch_i, size_t & i2, std::string nickname, std::vector<User> & uservect)
{
	if (i2 >= cmd.params.size() || cmd.params[i2] == "")
	{
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 461 ") + nickname + " " + cmd.name + " :Not enough parameters\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	if (!ChannelPassIsValid(cmd.params[i2]))
	{
		//:irc.example.com 696 mario #chat k p@ss :Invalid key parameter
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 696 ") + nickname + " " + cmd.params[0] + " k " + cmd.params[i2] + " :Invalid key parameter\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	channelvect[ch_i].setPass(cmd.params[i2]);
	i2++;
	return true;
}

bool addNewOperatorToChannel(Command & cmd, int fd, std::vector<Channel> & channelvect, std::vector<User> & uservect, size_t & ch_i, size_t & i2, std::string nickname)
{
	if (i2 >= cmd.params.size() || cmd.params[i2] == "")
	{
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 461 ") + nickname + " " + cmd.name + " :Not enough parameters\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	//std::cout << " param i2: " << cmd.params[i2] << " i2: " << i2 << std::endl; 
	size_t newoperatorindex = searchVectWithNick(uservect, cmd.params[i2]);
	//std::cout << " fddddddd: " << i2 << std::endl; 
	if (newoperatorindex == uservect.size())
	{
		//:server 401 <nick> Dan :No such nick/channel
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 401 ") + nickname + " " + cmd.params[i2] + " :No such nick/channel\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	int newoperatorfd = uservect[newoperatorindex].getFd();
	if (!channelvect[ch_i].userIsInChannel(newoperatorfd))
	{
		//:server 441 <nick> Dan #chat :They aren't on that channel
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 441 ") + nickname + " " + cmd.params[i2] + " " + cmd.params[0] + " :They aren't on that channel\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	channelvect[ch_i].addToOperators(newoperatorfd);
	i2++;
	return true;
}

bool removeOperatorFromChannel(Command & cmd, int fd, std::vector<Channel> & channelvect, std::vector<User> & uservect, size_t & ch_i, size_t & i2, std::string nickname)
{
	if (i2 >= cmd.params.size() || cmd.params[i2] == "")
	{
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 461 ") + nickname + " " + cmd.name + " :Not enough parameters\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	size_t operatortoremoveindex = searchVectWithNick(uservect, cmd.params[i2]);
	if (operatortoremoveindex == uservect.size())
	{
		//:server 401 <nick> Dan :No such nick/channel
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 401 ") + nickname + " " + cmd.params[i2] + " :No such nick/channel\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	int operatortoremovefd = uservect[operatortoremoveindex].getFd();
	if (!channelvect[ch_i].userIsInChannel(operatortoremovefd))
	{
		//:server 441 <nick> Dan #chat :They aren't on that channel
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 441 ") + nickname + " " + cmd.params[i2] + " " + cmd.params[0] + " :They aren't on that channel\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	if (!channelvect[ch_i].userIsInOperators(operatortoremovefd))
	{
		//::server 696 <nick> #chat o Bob :Invalid mode parameter
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 696 ") + nickname + " " + cmd.params[0] + " o " + cmd.params[i2] + " :Invalid mode parameter\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	if (channelvect[ch_i].getOperatorsSize() == 1)
	{
		//:irc.server.net 482 mionick #canale :You're the last channel operator
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 482 ") + nickname + " " + cmd.params[0] + " :You're the last channel operator\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	channelvect[ch_i].removeFromOperators(operatortoremovefd);
	i2++;
	return true;
}	

bool isAValidNumber(std::string numb, size_t & newnumb)
{
	if (numb.empty() || numb == "")//controllo ridondante ma vabbe
		return false;
	if (numb[0] == '+' && numb.size() == 1)
		return false;
	size_t i = 0;
	if (numb[0] == '+')
		i++;
	while(i < numb.size())
	{
		if (!(numb[i] >= '0' && numb[i] <= '9'))
			return false;
		i++;
	}

	char* end;
	unsigned long val = std::strtoul(numb.c_str(), &end, 10);

	// check for conversion errors
	if (*end != '\0' || end == numb.c_str())
		return false;

	// check for overflow (strtoul returns ULONG_MAX on overflow)
	if (val == ULONG_MAX && errno == ERANGE)
		return false;

	// check that it fits into size_t 
	if (val > std::numeric_limits<size_t>::max())
		return false;

	newnumb = static_cast<size_t>(val);
	return true;
}


bool SetNewUserlimitForChannel(Command & cmd, int fd, std::vector<Channel> & channelvect, std::vector<User> & uservect, size_t & ch_i, size_t & i2, std::string nickname)
{
	if (i2 >= cmd.params.size() || cmd.params[i2] == "")
	{
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 461 ") + nickname + " " + cmd.name + " :Not enough parameters\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	size_t newlimit;
	if (!isAValidNumber(cmd.params[i2], newlimit))
	{
		//:server 696 <nick> #channel l <number> :Invalid limit
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 696 ") + nickname + " " + cmd.name + " l " + cmd.params[i2] + " :Invalid limit\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}

	size_t vectsize = channelvect[ch_i].getUsersSize();
	if (vectsize > newlimit)
	{
		//:server 696 <nick> #channel l <number> :Invalid limit
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 696 ") + nickname + " " + cmd.name + " l " + cmd.params[i2] + " :Invalid limit\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	channelvect[ch_i].setUserLimit(newlimit);
	i2++;
	return true;
}

bool positivemode(Command & cmd, int fd, std::vector<Channel> & channelvect, std::vector<User> & uservect, size_t & ch_i, size_t & i2, std::string nickname, size_t & z, size_t & j)
{
	if (cmd.params[z][j] == 'i')
	{
		channelvect[ch_i].setIsInviteOnly(true);
	}
	else if (cmd.params[z][j] == 't')
	{
		channelvect[ch_i].setIsTopicRestricted(true);
	}
	else if (cmd.params[z][j] == 'k')
	{
		if (!addNewPassToChannel(cmd, fd, channelvect, ch_i, i2, nickname, uservect))
			return false;
	}
	else if (cmd.params[z][j] == 'o')
	{
		if (!addNewOperatorToChannel(cmd, fd, channelvect, uservect, ch_i, i2, nickname))
			return false;
	}
	else if (cmd.params[z][j] == 'l')
	{
		if (!SetNewUserlimitForChannel(cmd, fd, channelvect, uservect, ch_i, i2, nickname))
			return false;
	}
		else
	{
		//:server 472 <nick> l :is unknown mode char to me
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 472 ") + nickname + " " + cmd.params[z][j - 1] + " :is unknown mode char to me\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	return true;
}

bool negativemode(Command & cmd, int fd, std::vector<Channel> & channelvect, std::vector<User> & uservect, size_t & ch_i, size_t & i2, std::string nickname, size_t & z, size_t & j)
{
	if (cmd.params[z][j] == 'i')
	{
		channelvect[ch_i].setIsInviteOnly(false);
	}
	else if (cmd.params[z][j] == 't')
	{
		channelvect[ch_i].setIsTopicRestricted(false);
	}
	else if (cmd.params[z][j] == 'k')
	{
		channelvect[ch_i].setPass("");
	}
	else if (cmd.params[z][j] == 'o')
	{
		if (!removeOperatorFromChannel(cmd, fd, channelvect, uservect, ch_i, i2, nickname))
			return false;
	}
	else if (cmd.params[z][j] == 'l')
	{
		channelvect[ch_i].setUserLimit(-1);
	}
		else
	{
		//:server 472 <nick> l :is unknown mode char to me
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 472 ") + nickname + " " + cmd.params[z][j - 1] + " :is unknown mode char to me\r\n";
		safe_send(uservect, fd, reply);
		return false;
	}
	return true;
}

void execMode(Command & cmd, int fd, std::vector<User> & uservect, std::vector<Channel> & channelvect)
{
	size_t i = searchVectWithFd(uservect, fd);

	std::string nickname = uservect[i].getNickName();
	if (nickname == "")
		nickname = "*";

	if (!argumentsArePresent_mod(cmd, 1, nickname))
	{
		//461 <nick> MODE :Not enough parameters
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 461 ") + nickname + " " + cmd.name + " :Not enough parameters\r\n";
		safe_send(uservect, fd, reply);
		return ;
	}

	std::string channelname;
	if (!cmd.params.empty())
		channelname = cmd.params[0];
	else
		channelname = cmd.trailing;

	if (channelNameIsInvalid(cmd, fd, channelname, nickname, uservect))
		return ;
	
	size_t ch_i = searchChannel(channelvect, channelname);
	if (ch_i >= channelvect.size())
	{
		//:server 403 <nick> #nonexistent :No such channel
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 403 ") + nickname + " " + cmd.params[0] + " :No such channel\r\n";
		safe_send(uservect, fd, reply);
		return ;
	}

	if (!channelvect[ch_i].userIsInChannel(fd))
	{
		//:server 442 <nick> #chat :You're not on that channel
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 442 ") + nickname + " " + cmd.params[0] + " :You're not on that channel\r\n";
		safe_send(uservect, fd, reply);
		return ;
	}

	if (!argumentsArePresent_mod(cmd, 2, nickname))
	{
		//caso in cui c'e solo nome canale e si mostrano i permessi attuali
		//:server 324 <nick> #chat +ntkl password123 25
		getCurrentChannelStatus(nickname, channelname, ch_i, channelvect, fd, uservect);
		return ;
	}

	if (!channelvect[ch_i].searchFDinOperators(fd))
	{
		//:server 482 <nick> #chat :You're not channel operator
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 482 ") + nickname + " " + cmd.params[0] + " :You're not channel operator\r\n";
		safe_send(uservect, fd, reply);
		return ;
	}

	//fare il ciclo principale che fa i vari + - i t k l o
	cmd.params.push_back(cmd.trailing);

	if (cmd.params[1][0] != '+' && cmd.params[1][0] != '-')
	{
		//:server 472 <nick> l :is unknown mode char to me
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 472 ") + nickname + " " + cmd.params[0][0] + " :is unknown mode char to me\r\n";
		safe_send(uservect, fd, reply);
		return ;
	}

	size_t first_nonmode_param = 1;
	while (cmd.params[first_nonmode_param][0] == '+' || cmd.params[first_nonmode_param][0] == '-')
		first_nonmode_param++;
	size_t i2 = first_nonmode_param;
	size_t z = 1;
	bool isplus;
	while(z < first_nonmode_param)
	{
		size_t j = 0;
		while(j < cmd.params[z].size())
		{

			if (cmd.params[z][j] == '+')
			{
				isplus = true;
				j++;
				if (j == cmd.params[z].size())
				{
					//:server 472 <nick> l :is unknown mode char to me
					std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 472 ") + nickname + " " + cmd.params[z][j - 1] + " :is unknown mode char to me\r\n";
					safe_send(uservect, fd, reply);
					return ;
				}
				if (!positivemode(cmd, fd, channelvect, uservect, ch_i, i2, nickname, z, j))
					return ;

			}
			else if (cmd.params[z][j] == '-')
			{
				isplus = false;
				j++;
				if (j == cmd.params[z].size())
				{
					//:server 472 <nick> l :is unknown mode char to me
					std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 472 ") + nickname + " " + cmd.params[z][j - 1] + " :is unknown mode char to me\r\n";
					safe_send(uservect, fd, reply);
					return ;
				}
				if (!negativemode(cmd, fd, channelvect, uservect, ch_i, i2, nickname, z, j))
					return ;
			}
			else if (cmd.params[z][j] == 'i' || cmd.params[z][j] == 't' || cmd.params[z][j] == 'k' || cmd.params[z][j] == 'o' || cmd.params[z][j] == 'l')
			{
				if (isplus)
				{
					if (!positivemode(cmd, fd, channelvect, uservect, ch_i, i2, nickname, z, j))
						return ;
				}
				else
				{
					if (!negativemode(cmd, fd, channelvect, uservect, ch_i, i2, nickname, z, j))
						return ;
				}
			}
			else
			{
				//:server 472 <nick> l :is unknown mode char to me
				std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 472 ") + nickname + " " + cmd.params[z][j - 1] + " :is unknown mode char to me\r\n";
				safe_send(uservect, fd, reply);
				return ;
			}
			j++;
		}
		z++;
	}
}
