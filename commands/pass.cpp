
#include "../irc.hpp"

void execPass(Command cmd, int fd, std::vector<User> & uservect, std::string realpassword)
{
	if (!argumentsArePresent(cmd, 1))
		return ;

	std::string newpassword;
	if(cmd.params.size() > 0)
		newpassword = cmd.params[1];
	else
		newpassword = cmd.trailing;

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
