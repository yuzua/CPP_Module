#include "Dog.hpp"

#include "Brain.hpp"

#include <iostream>

Dog::Dog(void) : Animal("Dog"), brain_(new Brain()) {
    std::cout << "Dog is constructed." << std::endl;
}

Dog::Dog(Dog const &other) : Animal(other), brain_(new Brain(*other.brain_)) {
    std::cout << "Dog is copy-constructed." << std::endl;
}

Dog::~Dog(void) {
    std::cout << "Dog is destructed." << std::endl;
    delete this->brain_;
}

Dog &Dog::operator=(Dog const &other) {
    if (this != &other) {
        Animal::operator=(other);
        *this->brain_ = *other.brain_;
    }
    std::cout << "Dog is copy-assigned." << std::endl;
    return *this;
}

void Dog::makeSound(void) const {
    std::cout << "Dog barks: Woof" << std::endl;
}

void Dog::setIdea(int index, std::string const &idea) {
    this->brain_->setIdea(index, idea);
}

std::string const &Dog::getIdea(int index) const {
    return this->brain_->getIdea(index);
}
