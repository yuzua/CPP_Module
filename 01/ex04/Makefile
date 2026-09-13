CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = replace
SRCS = main.cpp replace.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)
	rm -f tests/tmp tests/out.txt
	rm -rf tests/tmp.replace

re: fclean all

test: $(NAME)
	bash tests/run_tests.sh

.PHONY: all clean fclean re test
