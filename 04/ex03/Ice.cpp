#include "Ice.hpp"

#include "ICharacter.hpp"

#include <iostream>

Ice::Ice(void) : AMateria("ice") {}

Ice::Ice(Ice const &other) : AMateria(other) {}

Ice::~Ice(void) {}

Ice &Ice::operator=(Ice const &other) {
    if (this != &other)
        AMateria::operator=(other);
    return *this;
}

AMateria *Ice::clone(void) const {
    return new Ice(*this);
}

void Ice::use(ICharacter &target) {
    std::cout << "* shoots an ice bolt at " << target.getName() << " *" << std::endl;
}
