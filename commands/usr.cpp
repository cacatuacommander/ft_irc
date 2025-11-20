#include "../irc.hpp"

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

	if (!argumentsArePresent(cmd, 1, uservect[i].getNickName(), fd))
		return ;

	std::string newusername;
	if(cmd.params.size() > 0)
		newusername = cmd.params[1];
	else
		newusername = cmd.trailing;

	if (i < uservect.size())
	{
		if (usernameAlredySet(i, uservect, fd))
			return ;
		uservect[i].setUserName(newusername);
		uservect[i].setIsVerified();
		//fare tutti i send di benvenuto
		//std::cout << "user aggiornato a:" << newusername << std::endl;
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}