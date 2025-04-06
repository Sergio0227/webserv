
CLIENT_SRC = Client/Client.cpp Socket/Socket.cpp main_client.cpp utils.cpp
SERVER_SRC = Server/HttpServer.cpp Socket/Socket.cpp main_server.cpp utils.cpp
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
CLIENT_EXEC = client
SERVER_EXEC = server
CFLAGS		= -Werror -Wall -Wextra
CPPFLAGS	= -std=c++98 -I./Socket -I./Client -I./Server -I.
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

all: $(CLIENT_EXEC) $(SERVER_EXEC)

$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) -o $(CLIENT_EXEC)

$(SERVER_EXEC): $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o $(SERVER_EXEC)

%.o: %.cpp
	@echo "Compiling $< with flags \"${CFLAGS}\" and \"${CPPFLAGS}\""
	@${CC} ${CFLAGS} ${CPPFLAGS} -c $< -o $@
	@echo "$(GREEN)Compilation of $< success$(DEF_COLOR)"

clean:
	@${RM} ${CLIENT_OBJ} ${SERVER_OBJ}
	@echo -n "$(CYAN)[$(NAME)]:$(DEF_COLOR) object files$(RED) \t => Removed!$(DEF_COLOR)\n";

fclean: clean
	@echo -n "$(CYAN)[$(NAME)]:$(DEF_COLOR) exec. files$(RED)  => Removed!$(DEF_COLOR)\n";
	@${RM} ${CLIENT_EXEC} ${SERVER_EXEC};


re: fclean all

.PHONY: all clean fclean re
