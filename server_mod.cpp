
#include "irc.hpp"

volatile bool g_running = true;

void handle_sigint(int)
{
	g_running = false;
}

bool isNumber(const char *str)
{
	if (!str || !*str)
	{
		std::cerr << "Error: port must be a number\n";
		return false;
	}
	int i;
	for (i = 0; str[i]; ++i)
	{
		if (str[i] < '0' || str[i] > '9')
		{
			std::cerr << "Error: port must be a number\n";
			return false;
		}
	}
	if (i > 5)
	{
		std::cerr << "Error: port out of range (1-65535)\n";
		return false;
	}
	return true;
}

bool port_parsing(const char *str, long & port )
{
	if (!isNumber(str))
		return false;

	port = std::strtol(str, NULL, 10);

	if (port < 1 || port > 65535)
	{
		std::cerr << "Error: port out of range (1-65535)\n";
		return false;
	}
	return true;
}

bool isValidPassword(const char *str)
{
	if (!str || !*str)
	{
		std::cerr << "Invalid Password\n";
		return false;
	}
	int i;
	for (i = 0; str[i]; ++i)
	{
		if (str[i] <= ' ' || str[i] == ':' || str[i] == ',')
		{
			std::cerr << "Invalid Password\n";
			return false;
		}
	}
	if (i > 510)
	{
		std::cerr << "Invalid Password\n";
		return false;
	}
	return true;
}

int main(int argc, char** argv)
{
	int server_fd;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);
 

	//gestione cntrl-C
	struct sigaction sa;
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);


	if (argc < 3)
	{
		std::cerr << "missing argument port and password needed" << std::endl;
		return 0;
	}

	if (!isValidPassword(argv[2]))
		return 1;
	std::string password = argv[2];
	long port;
	if (!port_parsing(argv[1], port))
		return 1;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		std::cerr << "socket() failed\n";
		return 1;
	}

	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(static_cast<uint16_t>(port));

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		std::cerr << "bind() failed\n";
		return 1;
	}

	if (listen(server_fd, MAX_CLIENTS) < 0)
	{
		std::cerr << "listen() failed\n";
		return 1;
	}

	std::cout << "Server listening on port " << port << "...\n";

	std::vector<pollfd> fds;
	fds.push_back((pollfd){ server_fd, POLLIN, 0 });

	char buffer[1024];

	std::vector<User> uservect;
	std::vector<Channel> channelvect;

	while (g_running)
	{
		int poll_count = poll(&fds[0], fds.size(), -1);
		if (poll_count < 0)
		{
			break;
		}

		for (size_t i = 0; i < fds.size(); ++i)
		{
			if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				size_t ind = searchVectWithFd(uservect, fds[i].fd);
				if (ind != uservect.size())
				{
					uservect.erase(uservect.begin() + ind);
					std::string msg = ":" + uservect[ind].getNickName() + " QUIT : Client exited\r\n";
					deleteFromGroups(channelvect, fds[i].fd, uservect, msg);
				}
				close(fds[i].fd);
				std::cout << "Client disconnected (fd=" << fds[i].fd << ")\n";
				fds.erase(fds.begin() + i);
				--i;
			}
			else if (fds[i].revents & POLLIN)
			{
				// a) Server socket → new connection
				if (fds[i].fd == server_fd)
				{
					int new_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
					if (new_fd >= 0)
					{
						std::cout << "New client connected (fd=" << new_fd << ")\n";
						fds.push_back((pollfd){ new_fd, POLLIN, 0 });
						std::string user_ip = inet_ntoa(address.sin_addr);
						uservect.push_back(User(new_fd, user_ip));
					}
				}
				// b) Client socket → incoming message
				else
				{
					std::memset(buffer, 0, sizeof(buffer));
					ssize_t bytes = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (bytes <= 0)
					{
						size_t ind = searchVectWithFd(uservect, fds[i].fd);
						if (ind != uservect.size())
						{
							uservect.erase(uservect.begin() + ind);
							std::string msg;
							if (bytes == 0) 
		  						msg = ":" + uservect[ind].getNickName() + " QUIT :Client exited\r\n";
							else
								msg = ":" + uservect[ind].getNickName() + " QUIT :Connection lost\r\n";
							deleteFromGroups(channelvect, fds[i].fd, uservect, msg);
						}
						close(fds[i].fd);
						if (bytes == 0)
							std::cout << "Client disconnected (fd=" << fds[i].fd << ")\n";
						else
							std::cout << "Client unexpectedly disconnected (fd=" << fds[i].fd << ")\n";
						fds.erase(fds.begin() + i);
						--i;
					}
					else
					{
						//std::cout << "Client " << fds[i].fd << ": " << buffer;
						
						//std::cout << "\nbuffer: " << buffer << std::endl;

						int index = searchVectWithFd(uservect, fds[i].fd);

						uservect[index].bufferAppend(buffer, bytes);
						size_t pos;
						std::cout << "\n\nstringa di input : " << uservect[index].getBuffer() << std::endl;
						while ((pos = uservect[index].bufferFind("\r\n")) != std::string::npos)
						{
							std::string line = uservect[index].bufferSubstr(0, pos);
							uservect[index].bufferErase(0, pos + 2);

							//std::cout << "\nline: " << line << std::endl;
							Command cmd = Parser::parse(line, uservect, fds[i].fd);
							if ( cmd.valid == true)
							{
								/* std::cout << " name: " << cmd.name << " params: "; 
								std::vector<std::string>::iterator it;
								for (it = cmd.params.begin(); it != cmd.params.end(); ++it)
								{
									std::cout << it->c_str();
								}
								std::cout << " trailing: " << cmd.trailing << " valid: " << cmd.valid << std::endl; */
								
								
								std::cerr << "fd: " << uservect[index].getFd() <<  std::endl;
								std::cerr << "nick: " << uservect[index].getNickName() <<  std::endl;
								std::cerr << "user: " << uservect[index].getUserName() <<  std::endl;
								std::cerr << "password: " << uservect[index].getPassword() <<  std::endl; 

/* 								Command cmd;
								cmd.name = line;
								cmd.params.push_back(line.substr(2, password.size()));
								cmd.valid = true;
								
								if (line[0] == 'P')
									execPass(cmd, fds[i].fd, uservect, password);
								else if (line[0] == 'N')
									execNick(cmd, fds[i].fd, uservect);
								else if (line[0] == 'U')
									execUser(cmd, fds[i].fd, uservect); */
								exec_command(cmd, uservect, channelvect, password, fds ,i);
							}
						}
						//std::string reply = "Server received: " + std::string(buffer);
						//safe_send(uservect, fds[i].fd, reply);
					}
				}
			}
			else if (fds[i].revents & POLLOUT && !uservect[i].sendBufferEmpty())
			{
				//std::cout << "quaaa " << std::endl;
				trySendBuffer(uservect, channelvect, fds, i);
			}
		}
	}

	for (size_t i = 0; i < fds.size(); ++i)
		close(fds[i].fd);
	return 0;
}
