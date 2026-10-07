#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "ICharacter.hpp"

class AMateria;

// スロットは空か、他と共有しないマテリア 1 個（INV6）。
// 具象の Ice / Cure は知らない。use は AMateria::use に渡すだけ。
class Character : public ICharacter {
    public:
        Character(void);
        explicit Character(std::string const &name);
        Character(Character const &other);
        virtual ~Character(void);
        Character &operator=(Character const &other);

        virtual std::string const &getName(void) const;
        virtual void               equip(AMateria *materia);
        virtual void               unequip(int idx);
        virtual void               use(int idx, ICharacter &target);

    private:
        static int const kSlotCount = 4;

        std::string name_;
        AMateria   *inventory_[kSlotCount];

        void nullInventory(void);
        void deleteInventory(void);
        void cloneInventoryFrom(Character const &other);
};

#endif
