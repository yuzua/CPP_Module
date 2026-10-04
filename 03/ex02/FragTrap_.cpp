#include "FragTrap.hpp"

#include <iostream>

const unsigned int FragTrap::kHitPoints;
const unsigned int FragTrap::kEnergyPoints;
const unsigned int FragTrap::kAttackDamage;

FragTrap::FragTrap(void)
    : ClapTrap("FragTrap", ClapTrap::kDefaultName,
               kHitPoints, kEnergyPoints, kAttackDamage) {
    std::cout << "FragTrap " << this->getName() << " is constructed." << std::endl;
}

FragTrap::FragTrap(std::string const &name)
    : ClapTrap("FragTrap", name, kHitPoints, kEnergyPoints, kAttackDamage) {
    std::cout << "FragTrap " << this->getName() << " is constructed." << std::endl;
}

FragTrap::FragTrap(FragTrap const &other) : ClapTrap(other) {
    std::cout << "FragTrap " << this->getName() << " is copy-constructed." << std::endl;
}

FragTrap::~FragTrap(void) {
    std::cout << "FragTrap " << this->getName() << " is destructed." << std::endl;
}

FragTrap &FragTrap::operator=(FragTrap const &other) {
    if (this != &other)
        ClapTrap::operator=(other);
    std::cout << "FragTrap " << this->getName() << " is copy-assigned." << std::endl;
    return *this;
}

void FragTrap::highFivesGuys(void) {
    std::cout << "FragTrap " << this->getName()
              << " raises a hand: high five, everyone!" << std::endl;
}
