
NAME = provaa
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC =	server_mod.cpp \
		autentication.cpp \
		user.cpp \
		utils.cpp \
		Parser.cpp \

all: $(NAME)

$(NAME): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(SRC)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
