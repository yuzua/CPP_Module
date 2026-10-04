CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME     = claptrap
SRCS     = main.cpp ClapTrap.cpp
OBJS     = $(SRCS:.cpp=.o)
HEADERS  = ClapTrap.hpp

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJS): $(HEADERS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
