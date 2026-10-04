#include "ClapTrap.hpp"
#include <climits>
#include <iostream>

ClapTrap::ClapTrap(void)
    : name_(""), hit_points_(10), energy_points_(10), attack_damage_(0) {
    std::cout << "ClapTrap " << name_ << " created" << std::endl;
}

ClapTrap::ClapTrap(std::string name)
    : name_(name), hit_points_(10), energy_points_(10), attack_damage_(0) {
    std::cout << "ClapTrap " << name_ << " created" << std::endl;
}

ClapTrap::ClapTrap(const ClapTrap &other)
    : name_(other.name_), hit_points_(other.hit_points_), energy_points_(other.energy_points_), attack_damage_(other.attack_damage_) {
    std::cout << "ClapTrap " << name_ << " copied" << std::endl;
}

ClapTrap::~ClapTrap() {
    std::cout << "ClapTrap " << name_ << " deleted" << std::endl;
}

ClapTrap &ClapTrap::operator=(const ClapTrap &other) {
    name_ = other.name_;
    hit_points_ = other.hit_points_;
    energy_points_ = other.energy_points_;
    attack_damage_ = other.attack_damage_;
    std::cout << "ClapTrap " << name_ << " copied" << std::endl;
    return *this;
}

void ClapTrap::attack(const std::string& target) {
    if (hit_points_ == 0) {
        std::cout << "ClapTrap " << name_ << " is already dead" << std::endl;
        return;
    }

    if (energy_points_ == 0) {
        std::cout << "ClapTrap " << name_ << " has no energy" << std::endl;
        return;
    }

    energy_points_ -= 1;
    std::cout << "ClapTrap " << name_ << " attacks " << target
              << ", causing " << attack_damage_ << " points of damage!" << std::endl;
}

void ClapTrap::takeDamage(unsigned int amount) {
    if (hit_points_ == 0) {
        std::cout << "ClapTrap " << name_ << " is already dead" << std::endl;
        return;
    }

    if (amount >= hit_points_) {
        hit_points_ = 0;
    } else {
        hit_points_ -= amount;
    }
    std::cout << "ClapTrap " << name_ << " takes " << amount << " points of damage!"
              << std::endl;
}

void ClapTrap::beRepaired(unsigned int amount) {
    if (hit_points_ == 0) {
        std::cout << "ClapTrap " << name_ << " is already dead" << std::endl;
        return;
    }

    if (energy_points_ == 0) {
        std::cout << "ClapTrap " << name_ << " has no energy" << std::endl;
        return;
    }

    energy_points_ -= 1;
    if (amount > UINT_MAX - hit_points_) {
        hit_points_ = UINT_MAX;
    } else {
        hit_points_ += amount;
    }
    std::cout << "ClapTrap " << name_ << " is repaired for " << amount << " points!"
              << std::endl;
}
