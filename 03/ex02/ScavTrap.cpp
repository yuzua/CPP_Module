#include "ScavTrap.hpp"

#include <iostream>

ScavTrap::ScavTrap()
    : ClapTrap("", 100, 50, 20) {
    std::cout << "ScavTrap " << name_ << " created" << std::endl;
}

ScavTrap::ScavTrap(std::string name)
    : ClapTrap(name, 100, 50, 20) {
    std::cout << "ScavTrap " << name_ << " created" << std::endl;
}

ScavTrap::ScavTrap(const ScavTrap &other)
    : ClapTrap(other) {
    std::cout << "ScavTrap " << name_ << " copied" << std::endl;
}

ScavTrap::~ScavTrap() {
    std::cout << "ScavTrap " << name_ << " deleted" << std::endl;
}

ScavTrap &ScavTrap::operator=(const ScavTrap &other) {
    ClapTrap::operator=(other);
    std::cout << "ScavTrap " << name_ << " assigned" << std::endl;
    return *this;
}

void ScavTrap::attack(const std::string& target) {
    if (hit_points_ == 0) {
        std::cout << "ScavTrap " << name_ << " is already dead" << std::endl;
        return;
    }

    if (energy_points_ == 0) {
        std::cout << "ScavTrap " << name_ << " has no energy" << std::endl;
        return;
    }

    energy_points_ -= 1;
    std::cout << "ScavTrap " << name_ << " attacks " << target
              << ", causing " << attack_damage_ << " points of damage!" << std::endl;
}

void ScavTrap::guardGate() {
    std::cout << "ScavTrap " << name_ << " is now in gatekeeper mode" << std::endl;
}