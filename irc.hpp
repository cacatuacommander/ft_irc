
#include "user.hpp"
#include <vector>
#include <string>

#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>

#define SERVER_NAME "our.server.irc"

std::vector<User>::iterator searchVectWithFd(std::vector<User> uservect, int fd);
std::vector<User>::iterator searchVectWithNick(std::vector<User> uservect, std::string nickname);

void execNick(std::string newnickname, int fd, std::vector<User> & uservect);
void execUser(std::string newnickname, int fd, std::vector<User> & uservect);
void execPass(std::string newpassword, int fd, std::vector<User> & uservect);