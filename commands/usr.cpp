
#include "../irc.hpp"

bool usernameAlredySet(std::vector<User>::iterator it, int fd)
{
	if (it->getIsVerified())
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 462 ") + it->getNickName() + " :You may not reregister\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return true;
	}
	return false;
}

void execUser(Command cmd, int fd, std::vector<User> & uservect)
{
	if (!argumentsArePresent(cmd, 1))
		return ;

	std::string newusername;
	if(cmd.params.size() > 0)
		newusername = cmd.params[1];
	else
		newusername = cmd.trailing;
	std::vector<User>::iterator it = searchVectWithFd(uservect, fd);

	if (it != uservect.end())
	{
		if (usernameAlredySet(it, fd))
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