
#include <sstream>

#include <sys/types.h>   // tipi di base (size_t, ssize_t, ...)
#include <sys/socket.h>  // socket(), bind(), listen(), accept()
#include <netinet/in.h>  // struct sockaddr_in, htons(), INADDR_ANY
#include <arpa/inet.h>   // inet_ntoa(), htons(), etc.
#include <fcntl.h>       // fcntl(), O_NONBLOCK
#include <unistd.h>      // close()
#include <cerrno>        // errno
#include <cstring>       // memset()
#include <iostream>      // std::cerr, std::cout

#include <cstdlib>
#include <poll.h>
#include <signal.h>

#include "Parser.hpp"
#include "channel.hpp"

#define SERVER_NAME "our.server.irc"

#define SERVER_CREATION_DATE "Wed Dec 10 2025"

#define MAX_CLIENTS 10

const std::string SERVER_VERSION = "our_ft_irc-1.0";

size_t searchVectWithFd(std::vector<User> & uservect, int fd);
size_t searchVectWithNick(std::vector<User> & uservect, std::string & nickname);
size_t searchChannel(std::vector<Channel> & chennelvect , std::string & channelname);

bool argumentsArePresent(Command cmd, std::vector<User> & uservect, unsigned int numbofargneeded, std::string nickname, int fd);
bool argumentsArePresent_mod(Command cmd, unsigned int numbofargneeded, std::string nickname);

bool channelNameIsInvalid(Command & cmd, int fd, std::string & channelname, std::string & nickname, std::vector<User> & uservect);

void execNick(Command cmd, int fd, std::vector<User> & uservect, std::vector<Channel> & channelvect);
void execUser(Command cmd, int fd, std::vector<User> & uservect);
void execPrivMsg(Command cmd, int fd, std::vector<Channel>& channelVect, std::vector<User> & uservect);
void execPass(Command cmd, int fd, std::vector<User> & uservect, std::string realpassword);
void execJoin(Command & cmd, int fd, std::vector<User> & uservect, std::vector<Channel> & channelvect);
void exec_command(Command cmd, std::vector<User> & uservect, std::vector<Channel> & channelvect, std::string serverpassword, std::vector<pollfd> &fds, int i);
void execKick(Command cmd, int fd, std::vector<Channel> & userchannel, std::vector<User> & uservect);
void execInvite(Command cmd, int fd, std::vector<Channel> & userchannel, std::vector<User> & uservect);
void execTopic(Command cmd, int fd, std::vector<Channel> & userchannel, std::vector<User> & uservect);
void execMode(Command & cmd, int fd, std::vector<User> & uservect, std::vector<Channel> & channelvect);
void execPart(Command &cmd, int fd, std::vector<User> & uservect, std::vector<Channel> & channelvect);
void execQuit(Command cmd, int fd, std::vector<User>& uservect, std::vector<Channel>& channelVect, std::vector<pollfd> &fds, int & i);
void execPing(Command cmd, int fd, std::vector<User> & uservect);
void execBotCall(Command cmd, int fd, std::vector<Channel>& channelVect, std::vector<User> & uservect);

void deleteFromGroups(std::vector<Channel> & channelvect, int fd, std::vector<User>& uservect, std::string msg);
void safe_send(std::vector<User> & uservect, int fd, std::string & toAdd);
void trySendBuffer(std::vector<User> & uservect, std::vector<Channel> & channelvect, std::vector<pollfd> & fds, size_t & fd_i, size_t us_i);

