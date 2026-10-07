#include "Animal.hpp"

#include <iostream>

Animal::Animal(void) : type("Animal") {
    std::cout << "Animal is constructed." << std::endl;
}

Animal::Animal(std::string const &type) : type(type) {
    std::cout << "Animal is constructed." << std::endl;
}

Animal::Animal(Animal const &other) : type(other.type) {
    std::cout << "Animal is copy-constructed." << std::endl;
}

Animal::~Animal(void) {
    std::cout << "Animal is destructed." << std::endl;
}

Animal &Animal::operator=(Animal const &other) {
    if (this != &other)
        this->type = other.type;
    std::cout << "Animal is copy-assigned." << std::endl;
    return *this;
}

std::string const &Animal::getType(void) const {
    return this->type;
}

void Animal::makeSound(void) const {
    std::cout << "Animal makes no particular sound." << std::endl;
}
