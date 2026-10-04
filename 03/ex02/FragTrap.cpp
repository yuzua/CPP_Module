#include "FragTrap.hpp"

#include <iostream>

FragTrap::FragTrap(void)
    : ClapTrap("", 100, 100, 30) {
    std::cout << "FragTrap " << name_ << " is constructed." << std::endl;
}

FragTrap::FragTrap(std::string name)
    : ClapTrap(name, 100, 100, 30) {
    std::cout << "FragTrap " << name_ << " is constructed." << std::endl;
}

FragTrap::FragTrap(const FragTrap &other) : ClapTrap(other) {
    std::cout << "FragTrap " << name_ << " is copied." << std::endl;
}

FragTrap::~FragTrap() {
    std::cout << "FragTrap " << name_ << " is destructed." << std::endl;
}

FragTrap &FragTrap::operator=(const FragTrap &other) {
    ClapTrap::operator=(other);
    std::cout << "FragTrap " << name_ << " is assigned." << std::endl;
    return *this;
}

void FragTrap::highFivesGuys(void) {
    std::cout << "Hi, " << name_ << "! Give me some skin!" << std::endl;
}
