#include "../irc.hpp"

void execQuit(Command cmd, int fd, std::vector<User>& uservect, std::vector<Channel>& channelVect, std::vector<pollfd> &fds, int & i)
{
	int ind = searchVectWithFd(uservect, fd);
	if (ind == -1)
		return;

	// Determina messaggio di quit
	std::string quittingMessage;
	if (!cmd.trailing.empty())
		quittingMessage = cmd.trailing;
	else
		quittingMessage = "Client exited";

	std::string msg = ":" + uservect[ind].getNickName() + uservect[ind].getUserName() + "@" + uservect[ind].getIp() + " QUIT :" + quittingMessage + "\r\n";
	size_t sizee = channelVect.size();
	for (size_t n = 0; n < sizee; ++n)
	{
		if (channelVect[n].userIsInChannel(fd))
		{
			channelVect[n].sendToAll(uservect, msg, fd);
			channelVect[n].removeFromUsers(fd);
			channelVect[n].removeFromOperators(fd);
			channelVect[n].removeFromInvites(fd);
			if (channelVect[n].getUsersSize() == 0 || (channelVect[n].getUsersSize() == 1 && channelVect[n].getListOfNicks(uservect) == "bot"))
			{
				channelVect.erase(channelVect.begin() + n);
				--n;
				--sizee;
			}
		}
	}
	close(fd);
	fds.erase(fds.begin() + i);
	--i;
	uservect.erase(uservect.begin() + ind);
}

