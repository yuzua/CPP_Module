#include "Brain.hpp"

#include <iostream>

int const Brain::kIdeaCount;

Brain::Brain(void) {
    std::cout << "Brain is constructed." << std::endl;
}

Brain::Brain(Brain const &other) {
    for (int i = 0; i < kIdeaCount; ++i)
        this->ideas[i] = other.ideas[i];
    std::cout << "Brain is copy-constructed." << std::endl;
}

Brain::~Brain(void) {
    std::cout << "Brain is destructed." << std::endl;
}

Brain &Brain::operator=(Brain const &other) {
    if (this != &other) {
        for (int i = 0; i < kIdeaCount; ++i)
            this->ideas[i] = other.ideas[i];
    }
    std::cout << "Brain is copy-assigned." << std::endl;
    return *this;
}

void Brain::setIdea(int index, std::string const &idea) {
    if (index < 0 || index >= kIdeaCount)
        return;
    this->ideas[index] = idea;
}

std::string const &Brain::getIdea(int index) const {
    static std::string const empty;

    if (index < 0 || index >= kIdeaCount)
        return empty;
    return this->ideas[index];
}
