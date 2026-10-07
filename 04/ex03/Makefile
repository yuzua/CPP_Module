CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME     = materia
SRCS     = main.cpp AMateria.cpp Ice.cpp Cure.cpp Character.cpp MateriaSource.cpp
OBJS     = $(SRCS:.cpp=.o)
HEADERS  = AMateria.hpp Ice.hpp Cure.hpp ICharacter.hpp IMateriaSource.hpp \
           Character.hpp MateriaSource.hpp

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
