
#include <vector>
#include <string>

#include <iostream>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>

#include "Parser.hpp"

#define SERVER_NAME "our.server.irc"

std::vector<User>::iterator searchVectWithFd(std::vector<User> uservect, int fd);
std::vector<User>::iterator searchVectWithNick(std::vector<User> uservect, std::string nickname);

void execNick(std::string newnickname, int fd, std::vector<User> & uservect);
void execUser(std::string newnickname, int fd, std::vector<User> & uservect);
void execPass(std::string newpassword, int fd, std::vector<User> & uservect);