CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME     = scavtrap
SRCS     = main.cpp ClapTrap.cpp ScavTrap.cpp
OBJS     = $(SRCS:.cpp=.o)
HEADERS  = ClapTrap.hpp ScavTrap.hpp

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
