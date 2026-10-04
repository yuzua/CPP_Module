#include "ScavTrap.hpp"

#include <iostream>

const unsigned int ScavTrap::kHitPoints;
const unsigned int ScavTrap::kEnergyPoints;
const unsigned int ScavTrap::kAttackDamage;

ScavTrap::ScavTrap(void)
    : ClapTrap("ScavTrap", ClapTrap::kDefaultName,
               kHitPoints, kEnergyPoints, kAttackDamage) {
    std::cout << "ScavTrap " << this->getName() << " is constructed." << std::endl;
}

ScavTrap::ScavTrap(std::string const &name)
    : ClapTrap("ScavTrap", name, kHitPoints, kEnergyPoints, kAttackDamage) {
    std::cout << "ScavTrap " << this->getName() << " is constructed." << std::endl;
}

// 基底のコピーコンストラクタを明示的に呼ぶ。省略すると ClapTrap() が走り、
// 名前も残量も複製されないまま「コンパイルは通る」バグになる。
ScavTrap::ScavTrap(ScavTrap const &other) : ClapTrap(other) {
    std::cout << "ScavTrap " << this->getName() << " is copy-constructed." << std::endl;
}

ScavTrap::~ScavTrap(void) {
    std::cout << "ScavTrap " << this->getName() << " is destructed." << std::endl;
}

ScavTrap &ScavTrap::operator=(ScavTrap const &other) {
    if (this != &other)
        ClapTrap::operator=(other);
    std::cout << "ScavTrap " << this->getName() << " is copy-assigned." << std::endl;
    return *this;
}

void ScavTrap::attack(const std::string &target) {
    // 行動の対価と拒否条件は ClapTrap が保証する。ここが持つ責務は演出だけ。
    if (!this->tryBeginAction())
        return;
    std::cout << "ScavTrap " << this->getName() << " fiercely attacks "
              << target << ", causing " << this->getAttackDamage()
              << " points of damage!" << std::endl;
}

void ScavTrap::guardGate(void) {
    std::cout << "ScavTrap " << this->getName() << " is now in Gate keeper mode."
              << std::endl;
}
