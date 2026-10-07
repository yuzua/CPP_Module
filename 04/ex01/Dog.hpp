#ifndef DOG_HPP
#define DOG_HPP

#include "Animal.hpp"
#include "Brain.hpp"
#include <string>
#include <iostream>

class Dog : public Animal
{
    private:
        Brain *brain_;
    public:
        Dog();
        Dog(const Dog &other);
        virtual ~Dog();
        Dog &operator=(const Dog &other);

        virtual void makeSound() const;
        void setIdea(int index, std::string const &idea);
        std::string const &getIdea(int index) const;
};
# endif