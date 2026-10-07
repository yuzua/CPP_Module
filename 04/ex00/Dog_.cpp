#include "Dog.hpp"

#include <iostream>

Dog::Dog(void) : Animal("Dog") {
    std::cout << "Dog is constructed." << std::endl;
}

Dog::Dog(Dog const &other) : Animal(other) {
    std::cout << "Dog is copy-constructed." << std::endl;
}

Dog::~Dog(void) {
    std::cout << "Dog is destructed." << std::endl;
}

Dog &Dog::operator=(Dog const &other) {
    if (this != &other)
        Animal::operator=(other);
    std::cout << "Dog is copy-assigned." << std::endl;
    return *this;
}

void Dog::makeSound(void) const {
    std::cout << "Dog barks: Woof" << std::endl;
}
