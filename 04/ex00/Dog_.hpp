#ifndef DOG_HPP
#define DOG_HPP

#include "Animal.hpp"

class Dog : public Animal {
    public:
        Dog(void);
        Dog(Dog const &other);
        virtual ~Dog(void);
        Dog &operator=(Dog const &other);

        virtual void makeSound(void) const;
};

#endif
