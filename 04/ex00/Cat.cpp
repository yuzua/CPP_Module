#include "Cat.hpp"

#include <iostream>

Cat::Cat() : Animal("Cat") {
    std::cout << "Cat created." << std::endl;
}

Cat::Cat(Cat const &other) : Animal(other) {
    std::cout << "Cat copied." << std::endl;
}

Cat::~Cat() {
    std::cout << "Cat destroyed." << std::endl;
}

Cat &Cat::operator=(Cat const &other) {
    if (this != &other)
        Animal::operator=(other);
    std::cout << "Cat assigned." << std::endl;
    return *this;
}

void Cat::makeSound() const {
    std::cout << "Cat: meow." << std::endl;
}
