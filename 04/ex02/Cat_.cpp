#include "Cat.hpp"

#include "Brain.hpp"

#include <iostream>

Cat::Cat(void) : Animal("Cat"), brain_(new Brain()) {
    std::cout << "Cat is constructed." << std::endl;
}

Cat::Cat(Cat const &other) : Animal(other), brain_(new Brain(*other.brain_)) {
    std::cout << "Cat is copy-constructed." << std::endl;
}

Cat::~Cat(void) {
    std::cout << "Cat is destructed." << std::endl;
    delete this->brain_;
}

Cat &Cat::operator=(Cat const &other) {
    if (this != &other) {
        Animal::operator=(other);
        *this->brain_ = *other.brain_;
    }
    std::cout << "Cat is copy-assigned." << std::endl;
    return *this;
}

void Cat::makeSound(void) const {
    std::cout << "Cat meows: Meow" << std::endl;
}

void Cat::setIdea(int index, std::string const &idea) {
    this->brain_->setIdea(index, idea);
}

std::string const &Cat::getIdea(int index) const {
    return this->brain_->getIdea(index);
}
