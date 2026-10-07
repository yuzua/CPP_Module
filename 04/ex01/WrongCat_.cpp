#include "WrongCat.hpp"

#include <iostream>

WrongCat::WrongCat(void) : WrongAnimal("WrongCat") {
    std::cout << "WrongCat is constructed." << std::endl;
}

WrongCat::WrongCat(WrongCat const &other) : WrongAnimal(other) {
    std::cout << "WrongCat is copy-constructed." << std::endl;
}

WrongCat::~WrongCat(void) {
    std::cout << "WrongCat is destructed." << std::endl;
}

WrongCat &WrongCat::operator=(WrongCat const &other) {
    if (this != &other)
        WrongAnimal::operator=(other);
    std::cout << "WrongCat is copy-assigned." << std::endl;
    return *this;
}

void WrongCat::makeSound(void) const {
    std::cout << "WrongCat hisses: Meow" << std::endl;
}
