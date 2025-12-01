
NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC =	server_mod.cpp \
		autentication.cpp \
		user.cpp \
		utils.cpp \
		Parser.cpp \
		commands/nick.cpp \
		commands/pass.cpp \
		commands/usr.cpp \
		commands/join.cpp \
		commands/privmsg.cpp \
		commands/kick.cpp \
		commands/invite.cpp \
		commands/topic.cpp \
		commands/mode.cpp \
		commands/part.cpp \
		commands/quit.cpp \
		channel.cpp \

all: $(NAME)

$(NAME): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(SRC)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
