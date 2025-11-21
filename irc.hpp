
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

#define SERVER_NAME ":our.server.irc"

#define SERVER_CREATION_DATE "Mon Nov 11 2024"

const std::string SERVER_VERSION = "our_ft_irc-1.0";

size_t searchVectWithFd(std::vector<User> & uservect, int fd);
size_t searchVectWithNick(std::vector<User> & uservect, std::string nickname);
bool argumentsArePresent(Command cmd, unsigned int numbofargneeded, std::string nickname, int fd);

void execNick(Command cmd, int fd, std::vector<User> & uservect);
void execUser(Command cmd, int fd, std::vector<User> & uservect);
void execPass(Command cmd, int fd, std::vector<User> & uservect, std::string realpassword);
void exec_command(Command cmd, std::vector<User> & uservect, int fd, std::string serverpassword);