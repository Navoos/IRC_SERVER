# TODO: if we want to push remove the debug rule
NAME = bin/ircserv
CC = g++
CFLAGS = -Wall -Wextra -Iinclude -std=c++98
LFLAGS = 
SRC = main.cpp socket.class.cpp server.class.cpp client.class.cpp mediator.class.cpp channel.class.cpp
SRCS = $(addprefix src/, $(SRC))
INC = socket.class.hpp server.class.hpp mediator.class.hpp command.interface.hpp client.class.hpp
INCLUDE = $(addprefix include/, $(INC))
OBJS = $(addprefix obj/, $(SRC:.cpp=.o))
ifeq ($(MAKECMDGOALS), debug)
	CFLAGS += -g3 -fsanitize=address,integer,undefined
	LFLAGS += -fsanitize=address,integer,undefined
else
	CFLAGS = -Iinclude
endif

all : $(NAME)

debug : fclean $(NAME)

obj/%.o : src/%.cpp $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME) : $(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

clean :
	/bin/rm -rf $(OBJS)

fclean : clean
	/bin/rm -rf $(NAME)

re : fclean all

.PHONY: all
