#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <string>
#include <iostream>

class Animal
{
    protected:
        std::string type_;
    public:
        Animal();
        Animal(const std::string &type_);
        Animal(const Animal &other);
        virtual ~Animal();
        Animal &operator=(const Animal &other);
        std::string getType() const;
        virtual void makeSound() const;
};

# endif