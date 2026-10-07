#include "Cat.hpp"

#include <iostream>

Cat::Cat() : Animal("Cat"), brain_(new Brain()) {
    std::cout << "Cat created." << std::endl;
}

Cat::Cat(Cat const &other) : Animal(other), brain_(new Brain(*other.brain_)) {
    std::cout << "Cat copied." << std::endl;
}

Cat::~Cat() {
    std::cout << "Cat destroyed." << std::endl;
    delete brain_;
}

Cat &Cat::operator=(Cat const &other) {
    if (this != &other)
    {
        Animal::operator=(other);
        *brain_ = *other.brain_;
    }
    std::cout << "Cat assigned." << std::endl;
    return *this;
}

void Cat::makeSound() const {
    std::cout << "Cat: meow." << std::endl;
}

void Cat::setIdea(int index, std::string const &idea) {
    brain_->setIdea(index, idea);
}

std::string const &Cat::getIdea(int index) const {
    return brain_->getIdea(index);
}
