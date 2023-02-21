NAME = bin/ircserv
CC = g++
CFLAGS = -Wall -Wextra -Iinclude
LFLAGS = 
SRC = main.cpp socket.class.cpp
SRCS = $(addprefix src/, $(SRC))
INCLUDE = $(addprefix include/, socket.class.hpp)
OBJS = $(addprefix obj/, $(SRC:.cpp=.o))
ifeq ($(MAKECMDGOALS), debug)
	CFLAGS += -g3 -fsanitize=address,integer,undefined
	LFLAGS += -fsanitize=address,integer,undefined
else
	CFLAGS += -O2
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
