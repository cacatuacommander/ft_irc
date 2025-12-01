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

    std::string msg = ":" + uservect[ind].getNickName() + " QUIT :" + quittingMessage + "\r\n";
    for (size_t n = 0; n < channelVect.size(); ++n)
    {
        if (channelVect[n].userIsInChannel(fd))
        {
            channelVect[n].sendToAll(msg, fd);
            channelVect[n].removeFromUsers(fd);
            channelVect[n].removeFromOperators(fd);
            channelVect[n].removeFromInvites(fd);
            if (channelVect[n].getUsersSize() == 0)
            {
                channelVect.erase(channelVect.begin() + n);
                --n;
            }
        }
    }
    close(fd);
	fds.erase(fds.begin() + i);
	--i;
    uservect.erase(uservect.begin() + ind);
}
