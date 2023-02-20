NAME = ircserv
CC = g++
CFLAGS = -Wall -Wextra -Werror
LFLAGS = 
SRC = main.cpp
SRCS = $(addprefix src/, $(SRC))
INCLUDE = 
OBJS = $(addprefix obj/, $(SRC:.cpp=.o))
ifeq ($(MAKECMDGOALS), debug)
	CFLAGS += -g -fsanitize=address,integer,undefined
	LFLAGS += -fsanitize=address,integer,undefined
else
	CFLAGS += -O2
endif

all : $(NAME)

debug : $(NAME)

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
