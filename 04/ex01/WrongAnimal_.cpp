#include "WrongAnimal.hpp"

#include <iostream>

WrongAnimal::WrongAnimal(void) : type("WrongAnimal") {
    std::cout << "WrongAnimal is constructed." << std::endl;
}

WrongAnimal::WrongAnimal(std::string const &type) : type(type) {
    std::cout << "WrongAnimal is constructed." << std::endl;
}

WrongAnimal::WrongAnimal(WrongAnimal const &other) : type(other.type) {
    std::cout << "WrongAnimal is copy-constructed." << std::endl;
}

WrongAnimal::~WrongAnimal(void) {
    std::cout << "WrongAnimal is destructed." << std::endl;
}

WrongAnimal &WrongAnimal::operator=(WrongAnimal const &other) {
    if (this != &other)
        this->type = other.type;
    std::cout << "WrongAnimal is copy-assigned." << std::endl;
    return *this;
}

std::string const &WrongAnimal::getType(void) const {
    return this->type;
}

void WrongAnimal::makeSound(void) const {
    std::cout << "WrongAnimal makes a generic wrong sound." << std::endl;
}
