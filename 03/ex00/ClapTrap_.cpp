#include "ClapTrap_.hpp"

#include <iostream>
#include <limits>

char const * const ClapTrap::kDefaultName = "Unnamed";

const unsigned int ClapTrap::kBaseHitPoints;
const unsigned int ClapTrap::kBaseEnergyPoints;
const unsigned int ClapTrap::kBaseAttackDamage;

ClapTrap::ClapTrap(void)
    : kind_("ClapTrap"),
      name_(kDefaultName),
      hitPoints_(kBaseHitPoints),
      energyPoints_(kBaseEnergyPoints),
      attackDamage_(kBaseAttackDamage) {
    std::cout << "ClapTrap " << this->name_ << " is constructed." << std::endl;
}

ClapTrap::ClapTrap(std::string const &name)
    : kind_("ClapTrap"),
      name_(name),
      hitPoints_(kBaseHitPoints),
      energyPoints_(kBaseEnergyPoints),
      attackDamage_(kBaseAttackDamage) {
    std::cout << "ClapTrap " << this->name_ << " is constructed." << std::endl;
}

ClapTrap::ClapTrap(std::string const &kind, std::string const &name,
                   unsigned int hitPoints, unsigned int energyPoints,
                   unsigned int attackDamage)
    : kind_(kind),
      name_(name),
      hitPoints_(hitPoints),
      energyPoints_(energyPoints),
      attackDamage_(attackDamage) {
    std::cout << "ClapTrap " << this->name_ << " is constructed." << std::endl;
}

ClapTrap::ClapTrap(ClapTrap const &other)
    : kind_(other.kind_),
      name_(other.name_),
      hitPoints_(other.hitPoints_),
      energyPoints_(other.energyPoints_),
      attackDamage_(other.attackDamage_) {
    std::cout << "ClapTrap " << this->name_ << " is copy-constructed." << std::endl;
}

ClapTrap::~ClapTrap(void) {
    std::cout << "ClapTrap " << this->name_ << " is destructed." << std::endl;
}

ClapTrap &ClapTrap::operator=(ClapTrap const &other) {
    if (this != &other) {
        this->kind_         = other.kind_;
        this->name_         = other.name_;
        this->hitPoints_    = other.hitPoints_;
        this->energyPoints_ = other.energyPoints_;
        this->attackDamage_ = other.attackDamage_;
    }
    std::cout << "ClapTrap " << this->name_ << " is copy-assigned." << std::endl;
    return *this;
}

std::string const &ClapTrap::getKind(void) const {
    return this->kind_;
}

std::string const &ClapTrap::getName(void) const {
    return this->name_;
}

unsigned int ClapTrap::getHitPoints(void) const {
    return this->hitPoints_;
}

unsigned int ClapTrap::getEnergyPoints(void) const {
    return this->energyPoints_;
}

unsigned int ClapTrap::getAttackDamage(void) const {
    return this->attackDamage_;
}

// 符号なし整数の引き算は 0 を下回ると 2^32 へ折り返す。
// 減算を必ずこの関数に通すことで、INV1 / INV2 を 1 箇所で保証する。
unsigned int ClapTrap::subtractClamped(unsigned int value, unsigned int amount) {
    if (amount >= value)
        return 0;
    return value - amount;
}

unsigned int ClapTrap::addClamped(unsigned int value, unsigned int amount) {
    unsigned int const headroom = std::numeric_limits<unsigned int>::max() - value;

    if (amount >= headroom)
        return std::numeric_limits<unsigned int>::max();
    return value + amount;
}

bool ClapTrap::tryBeginAction(void) {
    if (this->hitPoints_ == 0) {
        std::cout << this->kind_ << " " << this->name_
                  << " is destroyed and cannot act." << std::endl;
        return false;
    }
    if (this->energyPoints_ == 0) {
        std::cout << this->kind_ << " " << this->name_
                  << " has no energy point left and cannot act." << std::endl;
        return false;
    }
    this->energyPoints_ -= 1;
    return true;
}

void ClapTrap::attack(const std::string &target) {
    if (!this->tryBeginAction())
        return;
    std::cout << this->kind_ << " " << this->name_ << " attacks " << target
              << ", causing " << this->attackDamage_ << " points of damage!"
              << std::endl;
}

void ClapTrap::takeDamage(unsigned int amount) {
    if (this->hitPoints_ == 0) {
        std::cout << this->kind_ << " " << this->name_
                  << " is already destroyed and takes no further damage." << std::endl;
        return;
    }
    this->hitPoints_ = subtractClamped(this->hitPoints_, amount);
    std::cout << this->kind_ << " " << this->name_ << " takes " << amount
              << " points of damage! (hit points: " << this->hitPoints_ << ")"
              << std::endl;
    if (this->hitPoints_ == 0)
        std::cout << this->kind_ << " " << this->name_ << " is destroyed!" << std::endl;
}

void ClapTrap::beRepaired(unsigned int amount) {
    if (!this->tryBeginAction())
        return;
    this->hitPoints_ = addClamped(this->hitPoints_, amount);
    std::cout << this->kind_ << " " << this->name_ << " is repaired by " << amount
              << " points! (hit points: " << this->hitPoints_ << ")" << std::endl;
}
