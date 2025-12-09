#include "../irc.hpp"

bool passwordAlredyPresent(int fd, std::vector<User> & uservect, int i)
{
	if(!uservect[i].getPassword())
	{
		return false;
	}
	else
	{
		std::string temp = uservect[i].getNickName();
		if (temp == "")
			temp = "*";
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 462 ") + temp + " :You may not reregister\r\n";
		safe_send(uservect, fd, reply);
		return true;
	}
}

void execPass(Command cmd, int fd, std::vector<User> & uservect, std::string realpassword)
{
	size_t i = searchVectWithFd(uservect, fd);

	if (passwordAlredyPresent(fd, uservect, i))
		return ;
	if (!argumentsArePresent(cmd, uservect, 1, uservect[i].getNickName(), fd))
		return ;

	std::string newpassword;
	if(cmd.params.size() > 0)
		newpassword = cmd.params[0];
	else
		newpassword = cmd.trailing;


	if (newpassword == realpassword)
		uservect[i].setPassword();
	else
	{
		std::string reply = ":" + std::string(SERVER_NAME) + std::string(" 464 ") + "*" + " :Password incorrect\r\n";
		safe_send(uservect, fd, reply);
	}

}
