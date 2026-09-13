#include "HumanB.hpp"
#include <iostream>

HumanB::HumanB(std::string const &name) : _name(name), _weapon(NULL) {}

HumanB::~HumanB(void) {}

void HumanB::setWeapon(Weapon &weapon) {
    this->_weapon = &weapon;
}

void HumanB::attack(void) const {
    if (!this->_weapon) {
        std::cout << this->_name << " has no weapon" << std::endl;
        return;
    }

    std::cout << this->_name << " attacks with their " << this->_weapon->getType() << std::endl;
}
