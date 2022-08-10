SRC = main.cpp server/WebServer.cpp server/Servers.cpp\
	response/Response.cpp request/Request.cpp Config/Parser.cpp response/utils.cpp\
	Config/Config.cpp Config/Location.cpp

OBJ = $(SRC:.cpp=.o)

NAME = webserv

FLAGS = -g -Wall -Wextra -Werror -std=c++98



all: $(NAME)

$(NAME) : $(OBJ)
		@clang++ $(FLAGS) -o $(NAME) $(OBJ)
		@echo "\033[90m[\033[32mSuccess\033[90m]\033[32m Successfully compiled\033[0m"

clean:
		@rm -f $(OBJ)
		@echo "\033[90m[\033[91mDeleting\033[90m]\033[31m Object files deleted\033[0m"

fclean: clean
		@rm -f $(NAME)
		@echo "\033[90m[\033[91mDeleting\033[90m]\033[31m All created files deleted.\033[0m"

re:	fclean	all

.PHONY:			all clean fclean re