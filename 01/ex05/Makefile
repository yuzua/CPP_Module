CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = harl
SRCS = main.cpp Harl.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

test: $(NAME)
	bash tests/run_tests.sh

.PHONY: all clean fclean re test
