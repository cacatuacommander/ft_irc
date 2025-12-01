
#include "irc.hpp"

//int port = 6667;
#define PORT 6667
#define MAX_CLIENTS 10

volatile bool g_running = true;

void handle_sigint(int)
{
    g_running = false;
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


	if (argc < 2)//da modificare per aggiundere porta
	{
		std::cerr << "missing argument password needed" << std::endl;
		return 0;
	}

	std::string password = argv[1];//da modificare per aggiungere porta

	//port = atoi(argv[1]);//da controllare se argv1 e' numero

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
	address.sin_port = htons(PORT);//da modificare per aggiungere porta

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

	std::cout << "Server listening on port " << PORT << "...\n";

	std::vector<pollfd> fds;
	fds.push_back((pollfd){ server_fd, POLLIN, 0 });

	char buffer[1024];

	std::vector<User> uservect;
	std::vector<Channel> channelvect;

	while (g_running)
	{
		for (size_t i = 0; i < fds.size(); ++i)
		{
			fds[i].events = POLLIN;
			if (i != 0 && !uservect[searchVectWithFd(uservect, fds[i].fd)].send_buffer.empty())//migliorabile mettendo pollfd anche in User
				fds[i].events |= POLLOUT;
		}

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
					//levare utente anche da tutti i gruppi
					deleteFromGroups(channelvect, fds[i].fd);
				}
				close(fds[i].fd);
				std::cout << "Client disconnected (fd=" << fds[i].fd << ")\n";
				fds.erase(fds.begin() + i);
				--i;
			}
			if (fds[i].revents & POLLIN)
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
					if (bytes <= 0)//differenziare < 0 e == 0
					{
						//cancello utente da vettore utenti(da testare)
						int ind = searchVectWithFd(uservect, fds[i].fd);
						uservect.erase(uservect.begin() + ind);
						//levare utente anche da tutti i gruppi
						deleteFromGroups(channelvect, fds[i].fd);
						close(fds[i].fd);
						std::cout << "Client disconnected (fd=" << fds[i].fd << ")\n";
						fds.erase(fds.begin() + i);
						--i;
					}
					else
					{
						//std::cout << "Client " << fds[i].fd << ": " << buffer;
						
						//std::cout << "\nbuffer: " << buffer << std::endl;

						int index = searchVectWithFd(uservect, fds[i].fd);

						uservect[index].buffer.append(buffer, bytes);
						size_t pos;
						//std::cout << "stringa di input : " << uservect[index].buffer << std::endl;
						while ((pos = uservect[index].buffer.find("\n")) != std::string::npos)
						{

							std::string line = uservect[index].buffer.substr(0, pos);
							uservect[index].buffer.erase(0, pos + 2);

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
								
								
							/* 	std::cerr << "\nfd: " << uservect[index].getFd() <<  std::endl;
								std::cerr << "nick: " << uservect[index].getNickName() <<  std::endl;
								std::cerr << "user: " << uservect[index].getUserName() <<  std::endl;
								std::cerr << "password: " << uservect[index].getPassword() <<  std::endl;  */

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
						if (fds[i].revents & POLLOUT)
						{
							trySendBuffer(uservect, channelvect, fds, i);
						}
						//std::string reply = "Server received: " + std::string(buffer);
						//send(fds[i].fd, reply.c_str(), reply.size(), 0);
					}
				}
			}
		}
	}

	for (size_t i = 0; i < fds.size(); ++i)
		close(fds[i].fd);
	return 0;
}
