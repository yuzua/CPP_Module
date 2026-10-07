#include "WrongAnimal.hpp"

#include <iostream>

WrongAnimal::WrongAnimal() : type_("WrongAnimal") {
    std::cout << "WrongAnimal created." << std::endl;
}

WrongAnimal::WrongAnimal(const std::string &type) : type_(type) {
    std::cout << "WrongAnimal created." << std::endl;
}

WrongAnimal::WrongAnimal(const WrongAnimal &other) : type_(other.type_) {
    std::cout << "WrongAnimal copied." << std::endl;
}

WrongAnimal::~WrongAnimal() {
    std::cout << "WrongAnimal destroyed." << std::endl;
}

WrongAnimal &WrongAnimal::operator=(const WrongAnimal &other) {
    if (this != &other)
        type_ = other.type_;
    std::cout << "WrongAnimal assigned." << std::endl;
    return *this;
}

std::string WrongAnimal::getType() const {
    return type_;
}

void WrongAnimal::makeSound() const {
    std::cout << "WrongAnimal: hoge." << std::endl;
}
