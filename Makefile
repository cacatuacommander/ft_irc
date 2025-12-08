
NAME = ircserv
BONUS_NAME = ircbot

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC =		server_mod.cpp \
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
			commands/ping.cpp \
			commands/botCall.cpp \
			channel.cpp \

BONUS_SRC = bot/bot.cpp

all: $(NAME)

$(NAME): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(SRC)

clean:
	rm -f $(NAME)
	rm -f $(BONUS_NAME)

fclean: clean

re: fclean all

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(BONUS_SRC)
	$(CXX) $(CXXFLAGS) -o $(BONUS_NAME) $(BONUS_SRC)

.PHONY: all clean fclean re bonus
