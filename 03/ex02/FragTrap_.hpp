#ifndef FRAGTRAP_HPP
#define FRAGTRAP_HPP

#include "ClapTrap.hpp"

// 高耐久・高機動・高火力の ClapTrap。ハイタッチを要求できる。
//
// attack() は再定義しない。subject が要求しているのは構築・破棄メッセージの差だけで、
// 攻撃の演出を変える理由がないため、ClapTrap の実装をそのまま使う。
class FragTrap : public ClapTrap {
    public:
        static const unsigned int kHitPoints    = 100;
        static const unsigned int kEnergyPoints = 100;
        static const unsigned int kAttackDamage = 30;

        FragTrap(void);
        explicit FragTrap(std::string const &name);
        FragTrap(FragTrap const &other);
        ~FragTrap(void);
        FragTrap &operator=(FragTrap const &other);

        void highFivesGuys(void);
};

#endif
