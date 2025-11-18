#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>

#define PORT 6667
#define MAX_CLIENTS 10

// volatile bool g_running = true;

// void handle_sigint(int)
// {
//     g_running = false;
// }


int main()
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
					}
				}
				// b) Client socket → incoming message
				else
				{
					std::memset(buffer, 0, sizeof(buffer));
					ssize_t bytes = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (bytes <= 0)
					{
						std::cout << "Client disconnected (fd=" << fds[i].fd << ")\n";
						close(fds[i].fd);
						fds.erase(fds.begin() + i);
						--i;
					}
					else
					{
						std::cout << "Client " << fds[i].fd << ": " << buffer;
						std::string reply = "Server received: " + std::string(buffer);
						send(fds[i].fd, reply.c_str(), reply.size(), 0);
					}
				}
			}
		}
	}

	for (size_t i = 0; i < fds.size(); ++i)
        close(fds[i].fd);
	return 0;
}
