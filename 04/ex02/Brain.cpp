#include "Brain.hpp"

#include <iostream>

Brain::Brain() {
    std::cout << "Brain created." << std::endl;
}

Brain::Brain(const Brain &other) {
    std::cout << "Brain copied." << std::endl;
    for (int i = 0; i < size; i++)
        ideas_[i] = other.ideas_[i];
}

Brain::~Brain() {
    std::cout << "Brain destroyed." << std::endl;
}

Brain &Brain::operator=(const Brain &other) {
    if (this != &other)
    {
        for (int i = 0; i < size; i++)
            ideas_[i] = other.ideas_[i];
    }
    std::cout << "Brain assigned." << std::endl;
    return *this;
}

void Brain::setIdea(int index, std::string const &idea) {
    if (index < 0 || index >= size)
    {
        std::cout << "Index out of range." << std::endl;
        return;
    }
    ideas_[index] = idea;
}

std::string const &Brain::getIdea(int index) const {
    if (index < 0 || index >= size)
    {
        std::cout << "Index out of range." << std::endl;
        std::cout << "Returning first idea." << std::endl;
        return ideas_[0];
    }
    return ideas_[index];
}
