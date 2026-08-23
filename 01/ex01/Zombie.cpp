#include "Zombie.hpp"
#include <iostream>

Zombie::Zombie(void) {}

Zombie::Zombie(const std::string& name) : _name(name) {}

Zombie::~Zombie(void) {
	std::cout << this->_name << " is destroyed" << std::endl;
}

void Zombie::setName(const std::string& name) {
	this->_name = name;
}

void Zombie::announce(void) const {
    std::cout << this->_name + ": BraiiiiiiinnnzzzZ..." << std::endl;
}
