#ifndef DOG_HPP
#define DOG_HPP

#include "Animal.hpp"
#include <string>
#include <iostream>

class Dog : public Animal
{
    public:
        Dog();
        Dog(const Dog &other);
        virtual ~Dog();
        Dog &operator=(const Dog &other);

        virtual void makeSound() const;
};
# endif