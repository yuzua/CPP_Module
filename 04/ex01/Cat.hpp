#ifndef CAT_HPP
#define CAT_HPP

#include "Animal.hpp"
#include "Brain.hpp"
#include <string>
#include <iostream>

class Cat : public Animal {
    private:
        Brain *brain_;
    public:
        Cat();
        Cat(const Cat &other);
        virtual ~Cat();
        Cat &operator=(Cat const &other);

        virtual void makeSound() const;
        void setIdea(int index, std::string const &idea);
        std::string const &getIdea(int index) const;
};

#endif
