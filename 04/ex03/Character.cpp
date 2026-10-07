#include "Character.hpp"

#include "AMateria.hpp"

int const Character::kSlotCount;

Character::Character(void) : name_("default") {
    this->nullInventory();
}

Character::Character(std::string const &name) : name_(name) {
    this->nullInventory();
}

Character::Character(Character const &other) : name_(other.name_) {
    this->nullInventory();
    this->cloneInventoryFrom(other);
}

Character::~Character(void) {
    this->deleteInventory();
}

Character &Character::operator=(Character const &other) {
    if (this != &other) {
        this->name_ = other.name_;
        this->deleteInventory();
        this->cloneInventoryFrom(other);
    }
    return *this;
}

std::string const &Character::getName(void) const {
    return this->name_;
}

void Character::equip(AMateria *materia) {
    int i;

    if (materia == 0)
        return;
    for (i = 0; i < kSlotCount; ++i) {
        if (this->inventory_[i] == materia)
            return;
    }
    for (i = 0; i < kSlotCount; ++i) {
        if (this->inventory_[i] == 0) {
            this->inventory_[i] = materia;
            return;
        }
    }
}

void Character::unequip(int idx) {
    if (idx < 0 || idx >= kSlotCount)
        return;
    this->inventory_[idx] = 0;
}

void Character::use(int idx, ICharacter &target) {
    if (idx < 0 || idx >= kSlotCount || this->inventory_[idx] == 0)
        return;
    this->inventory_[idx]->use(target);
}

void Character::nullInventory(void) {
    for (int i = 0; i < kSlotCount; ++i)
        this->inventory_[i] = 0;
}

void Character::deleteInventory(void) {
    for (int i = 0; i < kSlotCount; ++i) {
        delete this->inventory_[i];
        this->inventory_[i] = 0;
    }
}

void Character::cloneInventoryFrom(Character const &other) {
    for (int i = 0; i < kSlotCount; ++i) {
        if (other.inventory_[i] != 0)
            this->inventory_[i] = other.inventory_[i]->clone();
    }
}
