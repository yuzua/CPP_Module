#include "Animal.hpp"

Animal::Animal() : type_("Animal") {
    std::cout << "Animal created." << std::endl;
}

Animal::Animal(const std::string &type_) : type_(type_) {
    std::cout << "Animal created." << std::endl;
}

Animal::Animal(const Animal &other) : type_(other.type_) {
    std::cout << "Animal copied." << std::endl;
}

Animal::~Animal() {
    std::cout << "Animal destroyed." << std::endl;
}

Animal &Animal::operator=(const Animal &other) {
    if (this != &other)
        type_ = other.type_;
    std::cout << "Animal assigned." << std::endl;
    return *this;
}

std::string Animal::getType() const {
    return type_;
}

void Animal::makeSound() const {
    std::cout << "Animal: hoge." << std::endl;
}
