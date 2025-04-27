SERVER_SRC = Server/HttpServer.cpp Socket/Socket.cpp main_server.cpp utils.cpp formatter.cpp Location/Location.cpp Brain/Brain.cpp ServerConfig/Config.cpp

SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

SERVER_EXEC = server
CFLAGS		= -Werror -Wall -Wextra
CPPFLAGS	= -std=c++98 -I./Socket -I./Client -I./Server -I./Location -I./Brain -I./ServerConfig -I.
CC			= c++
RM			= rm -rf

# Define colors
DEF_COLOR = \033[0;39m
ORANGE = \033[0;33m
GRAY = \033[0;90m
RED = \033[0;91m
GREEN = \033[1;92m
YELLOW = \033[1;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m

all: $(SERVER_EXEC)

$(SERVER_EXEC): $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o $(SERVER_EXEC)

%.o: %.cpp
	@echo "Compiling $< with flags \"${CFLAGS}\" and \"${CPPFLAGS}\""
	@${CC} ${CFLAGS} ${CPPFLAGS} -c $< -o $@
	@echo "$(GREEN)Compilation of $< success$(DEF_COLOR)"

clean:
	@${RM} ${SERVER_OBJ}
	@echo -n "$(CYAN)[$(NAME)]:$(DEF_COLOR) object files$(RED) \t => Removed!$(DEF_COLOR)\n";

fclean: clean
	@echo -n "$(CYAN)[$(NAME)]:$(DEF_COLOR) exec. files$(RED)  => Removed!$(DEF_COLOR)\n";
	@${RM} ${SERVER_EXEC};


re: fclean all

.PHONY: all clean fclean re
