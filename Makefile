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

SRC				= main.cpp FileParser.cpp Http.cpp Request.cpp Response.cpp \
						Server.cpp ServerLocation.cpp Utils.cpp WebServ.cpp

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

re: fclean all

run:
	clear
	@echo "\033[1;31m🧹 Clear all things...\033[0m"
	@make -s fclean
	@echo "\n\033[1;34m🚧 Compiling WebServ...\033[0m"
	@make -s
	@echo "\033[1;32m✔️ Ready!\033[0m"
	@echo "\n\033[1;35m🌐 Exec WebServ\033[0m"
	@./$(NAME) default.conf

valgrind: all
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./webserv default.conf

$(OBJDIR):
	mkdir -p objects

$(OBJDIR)/%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCPATH)

# **************************************************************************** #
# *----------------------------------Poetry----------------------------------* #
# **************************************************************************** #
POETRY_HOME	= /opt/poetry
POETRY = $(POETRY_HOME)/bin/poetry

poetry_install:  | $(POETRY_HOME)
$(POETRY_HOME):
#	Install Poetry: A Python dependency manager
	curl -sSL https://install.python-poetry.org | sudo POETRY_HOME=$(POETRY_HOME) python3 -

poetry_uninstall:
	curl -sSL https://install.python-poetry.org | sudo POETRY_HOME=$(POETRY_HOME) python3 - --uninstall

# **************************************************************************** #
# *----------------------------Virtual Environment---------------------------* #
# **************************************************************************** #
VENV_DIR = ./.venv # TODO instead of relative path, set folder equal to makefile location
venv_install: | poetry_install $(VENV_DIR)
$(VENV_DIR):
#	Update and add python support to create virtual environment
	sudo apt update && sudo apt install -y python3-pip python3.8-venv
	@echo "Creating Virtual Environment"
	python3 -m venv .venv
	$(POETRY) install

venv_uninstall:
	@$(RM) $(VENV_DIR)

# **************************************************************************** #
# *-----------------------------------tests----------------------------------* #
# **************************************************************************** #
test_install: poetry_install venv_install

test: test_install
	$(POETRY) run pytest

test_clean: poetry_uninstall venv_uninstall

test_re: test_clean test_install

-include $(DEPS)
