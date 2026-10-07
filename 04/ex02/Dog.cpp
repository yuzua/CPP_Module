#include "Dog.hpp"
#include <string>

Dog::Dog() : Animal("Dog") {
    std::cout << "Dog created." << std::endl;
    brain_ = new Brain();
}

Dog::Dog(const Dog &other) : Animal(other) {
    std::cout << "Dog copied." << std::endl;
    brain_ = new Brain(*other.brain_);
}

Dog::~Dog() {
    std::cout << "Dog destroyed." << std::endl;
    delete brain_;
}

Dog &Dog::operator=(const Dog &other) {
    if (this != &other)
    {
        Animal::operator=(other);
        *brain_ = *other.brain_;
    }
    std::cout << "Dog assigned." << std::endl;
    return *this;
}

void Dog::makeSound() const {
    std::cout << "Dog: Woof woof!." << std::endl;
}

void Dog::setIdea(int index, std::string const &idea) {
    brain_->setIdea(index, idea);
}

std::string const &Dog::getIdea(int index) const {
    return brain_->getIdea(index);
}
