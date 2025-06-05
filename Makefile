SRC = 	src/Server/HttpServer.cpp \
				src/Socket/Socket.cpp \
				src/main_server.cpp \
				src/utils.cpp \
				src/formatter.cpp \
				src/Location/Location.cpp \
				src/Brain/Brain.cpp \
				src/ServerConfig/Config.cpp \
				src/ClientInfo/ClientInfo.cpp \
				src/CGI/CGI.cpp \
				src/HttpResponse/HttpResponse.cpp

OBJS		= ${SRC:.cpp=.o}
NAME		= webserv
CFLAGS		= -Werror -Wall -Wextra
CPPFLAGS	= -std=c++98 -g -O0
INCLUDES	= -I./lib -g
				
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

all: ${NAME}

${NAME}: ${OBJS}
	@echo "$(ORANGE)[$(NAME)]:$(DEF_COLOR) Linking object files to create executable $(GREEN)${NAME}$(DEF_COLOR)"
	@${CC} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} ${OBJS} -o ${NAME}
	@echo "$(ORANGE)[$(NAME)]:$(DEF_COLOR) exec file $(GREEN)=> Created!$(DEF_COLOR)"
	@echo " $(CYAN)\
        	   ╔═════════════════════════════════════════╗ \n\
                   ║$(YELLOW)__        __   _         $(GREEN) BY MAX & SERGIO$(CYAN)║ \n\
                   ║$(YELLOW)\ \      / /__| |__  ___  ___ _ ____   __$(CYAN)║ \n\
                   ║$(YELLOW) \ \ /\ / / _ \ '_ \/ __|/ _ \ '__\ \ / /$(CYAN)║ \n\
                   ║$(YELLOW)  \ V  V /  __/ |_) \__ \  __/ |   \ V / $(CYAN)║ \n\
                   ║$(YELLOW)   \_/\_/ \___|_.__/|___/\___|_|    \_/  $(CYAN)║ \n\
                   ╚═════════════════════════════════════════╝"

%.o: %.cpp
	@echo "Compiling $< with flags \"${CFLAGS}\" and \"${CPPFLAGS}\""
	@${CC} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $< -o $@
	@echo "$(GREEN)Compilation of $< success$(DEF_COLOR)"

clean:
	@${RM} ${OBJS}
	@echo -n "$(CYAN)[$(NAME)]:$(DEF_COLOR) object files$(RED) \t => Removed!$(DEF_COLOR)\n";

fclean: clean
	@echo -n "$(CYAN)[$(NAME)]:$(DEF_COLOR) exec. files$(RED)  => Removed!$(DEF_COLOR)\n";
	@${RM} ${NAME};


re: fclean all

.PHONY: all clean fclean re