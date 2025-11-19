
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
bool argumentsArePresent(Command cmd, int numbofargneeded);

void execNick(Command cmd, int fd, std::vector<User> & uservect);
void execUser(Command cmd, int fd, std::vector<User> & uservect);
void execPass(Command cmd, int fd, std::vector<User> & uservect, std::string realpassword);
void exec_command(Command cmd, std::vector<User> uservect, int fd, std::string serverpassword);