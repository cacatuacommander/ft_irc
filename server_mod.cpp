
#include "irc.hpp"

#define PORT 6667
#define MAX_CLIENTS 10

// volatile bool g_running = true;

// void handle_sigint(int)
// {
//     g_running = false;
// }


int main(/* int argc, char** argv */)
{
	int server_fd;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);
 
    /* //gestione cntrl-C
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL); */

/* 	if (argc < 2 )
	{
		std::cerr << "missing argument password needed" << std::endl;
		return 0;
	}
	std::string password = argv[1]; */

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
	address.sin_port = htons(PORT);

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

	while (true)
	{
		int poll_count = poll(&fds[0], fds.size(), -1);
		if (poll_count < 0)
		{
			std::cerr << "poll() failed\n";
			break;
		}

		for (size_t i = 0; i < fds.size(); ++i)
		{
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
						uservect.push_back(User(new_fd));
					}
				}
				// b) Client socket → incoming message
				else
				{
					std::memset(buffer, 0, sizeof(buffer));
					ssize_t bytes = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (bytes <= 0)//differenziare < 0 e == 0
					{
						std::cout << "Client disconnected (fd=" << fds[i].fd << ")\n";
						close(fds[i].fd);
						fds.erase(fds.begin() + i);
						//cancellare oggetto utente e levarlo da tutti i gruppi
						--i;
					}
					else
					{
						//std::cout << "Client " << fds[i].fd << ": " << buffer;
						
						/* if (buffer[0] == 'P')
							execPass(&buffer[2], fds[i].fd, uservect);
						else if (buffer[0] == 'N')
							execNick(&buffer[2], fds[i].fd, uservect);
						else if (buffer[0] == 'U')
							execUser(&buffer[2], fds[i].fd, uservect); */
						std::cout << "buffer: " << buffer;
						Command cmd = Parser::parse(buffer, uservect, fds[i].fd);
						if (cmd.valid == true)
						{
							std::cout << "name: " << cmd.name << "params: "; 
							std::vector<std::string>::iterator it;
							for (it = cmd.params.begin(); it != cmd.params.end(); ++it)
							{
								std::cout << it->c_str();
							}
							std::cout << "trailing: " << cmd.trailing << "valid: " << cmd.valid << std::endl;
							//exec_command(cmd, uservect, fds[i].fd, password);
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
