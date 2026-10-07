#include "Cure.hpp"

#include "ICharacter.hpp"

#include <iostream>

Cure::Cure(void) : AMateria("cure") {}

Cure::Cure(Cure const &other) : AMateria(other) {}

Cure::~Cure(void) {}

Cure &Cure::operator=(Cure const &other) {
    if (this != &other)
        AMateria::operator=(other);
    return *this;
}

AMateria *Cure::clone(void) const {
    return new Cure(*this);
}

void Cure::use(ICharacter &target) {
    std::cout << "* heals " << target.getName() << "'s wounds *" << std::endl;
}
