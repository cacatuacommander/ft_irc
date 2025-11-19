
#include "irc.hpp"
#include <iostream>

void execNick(std::string newnickname, int fd, std::vector<User> & uservect)
{
	//se password c'e ed e' giusta lo controllo qui?
	//controllo che newnickname non sia "" lo faccio qui?
	std::vector<User>::iterator it;
	for (it = uservect.begin(); it != uservect.end(); ++it)
	{
		if (it->getFd() == fd)
			break;
	}
	
	if (newnickname == "" || newnickname.length() < 1)
	{
		std::string reply = std::string(SERVER_NAME) + std::string(" 431 ") + it->getNickName() + " :\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		return ;
	}

	if (it != uservect.end())
	{
		//CONTROLLARE CHE NON SIA GIA IN USO
		it->setNickName(newnickname);
		//std::cout << "nick aggiornato a:" << newnickname << std::endl;
		//scrivere send con successo operazione?
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}

void execUser(std::string newusername, int fd, std::vector<User> & uservect)
{
	//se password c'e ed e' giusta lo controllo qui?
	//controllo che newusername non sia "" lo faccio qui?
	//faccio controllo che anche nick debba gia esserci per poter mettere user? (MEGLIO DI SI FORSE)
	std::vector<User>::iterator it;
	for (it = uservect.begin(); it != uservect.end(); ++it)
	{
		if (it->getFd() == fd)
			break;
	}
	if (it != uservect.end())
	{
		it->setUserName(newusername);
		//std::cout << "user aggiornato a:" << newusername << std::endl;
		//scrivere send con successo operazione?
	}
	else
	{
		std::cerr << "Problemaa non trovato fd" << std::endl;
	}
}

void execPass(std::string newpassword, int fd, std::vector<User> & uservect, std::string realpassword)
{	
	//controllo che newpassword non sia "" lo faccio qui?
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
			//scrivere send con successo operazione?
		}
		else
		{
			//std::cerr << "ERR_ALREDYREGISTERED" << std::endl;
			//caso di nick o user gia inseriti (e quindi anche pass era stata gia inserita correttamente) lo faccio qui(?)
			//fare send di errore ERR_ALREADYREGISTRED
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