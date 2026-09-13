#ifndef HUMANB_HPP
#define HUMANB_HPP
#include <string>
#include "Weapon.hpp"

class HumanB {
    private:
        std::string _name;
        Weapon *_weapon;
    public:
        HumanB(std::string const &name);
        ~HumanB(void);
        void setWeapon(Weapon &weapon);
        void attack(void) const;
};

#endif
