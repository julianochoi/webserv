NAME			= webserv

Color_Off		= '\033[0m'       # Text Reset
BRed			= '\033[1;31m'    # Red
BGreen			= '\033[1;32m'    # Green
BYellow			= '\033[1;33m'    # Yellow
BBlue			= '\033[1;34m'    # Blue
BPurple			= '\033[1;35m'    # Purple
BCyan			= '\033[1;36m'    # Cyan


CC				= c++  
CFLAGS			= -g -Wall -Wextra -Werror -std=c++98 -MMD -MP -pedantic
RM 				= rm -rf

SRC				= main.cpp

INCPATH  		= -I./srcs -I./srcs/req -I./srcs/resp -I./srcs/serv 

OBJDIR			= objects
OBJ				= $(SRC:%.cpp=$(OBJDIR)/%.o)
DEPS			= $(SRC:%.cpp=$(OBJDIR)/%.d)

vpath %.cpp srcs srcs/req srcs/resp srcs/serv

all: $(NAME)

$(NAME): $(OBJDIR) $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

dep:
	which php-cgi || (sudo apt-get update && sudo apt-get install php-cgi)

clean:
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(NAME) 

re: 
	@fclean all

run:
	clear
	@echo "\033[1;31m🧹 Clear all things...\033[0m"
	@make -s fclean
	@echo "\n\033[1;34m🚧 Compiling WebServ...\033[0m"
	@make -s
	@echo "\033[1;32m✔️ Ready!\033[0m"
	@echo "\n\033[1;35m🌐 Exec WebServ\033[0m"
	@./$(NAME) default.conf


$(OBJDIR):
	mkdir -p objects

$(OBJDIR)/%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCPATH)

-include $(DEPS)
