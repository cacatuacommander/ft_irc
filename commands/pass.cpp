#include "../irc.hpp"

void execPass(Command cmd, int fd, std::vector<User> & uservect, std::string realpassword)
{
	size_t i = searchVectWithFd(uservect, fd);

	if (!argumentsArePresent(cmd, 1, uservect[i].getNickName(), fd))//Command cmd, unsigned int numbofargneeded, std::string nickname, int fd
		return ;

	std::string newpassword;
	if(cmd.params.size() > 0)
		{newpassword = cmd.params[0]; /* std::cerr << "ciaoo122" << std::endl; */}
	else
		{newpassword = cmd.trailing;}
	
	if (i < uservect.size())
	{
		if(!uservect[i].getPassword())
		{		/* std::cout << "ciaoo3" << std::endl; */
			if (newpassword == realpassword)
				uservect[i].setPassword();
			else
			{
				std::string reply = std::string(SERVER_NAME) + std::string(" 464 ") + "*" + " :Password incorrect\r\n";
				send(fd, reply.c_str(), reply.size(), 0);
			}		//std::cout << "ciaoo4" << std::endl;
		}
		else
		{	//std::cout << "ciaoo5" << std::endl;
			std::string temp = uservect[i].getNickName();
			if (temp == "")
				temp = "*";
			std::string reply = std::string(SERVER_NAME) + std::string(" 462 ") + temp + " :You may not reregister\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
		}	//std::cout << "ciaoo6" << std::endl;
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}
