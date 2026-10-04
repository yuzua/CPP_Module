#ifndef SCAVTRAP_HPP
#define SCAVTRAP_HPP

#include "ClapTrap.hpp"

// 高耐久・高火力の ClapTrap。ゲートキーパーモードを持つ。
//
// ClapTrap の不変条件 INV1〜INV5 をそのまま引き継ぐ。属性は protected コンストラクタ
// で「初期化」するだけで、構築後に書き換える経路を持たない。
class ScavTrap : public ClapTrap {
    public:
        static const unsigned int kHitPoints    = 100;
        static const unsigned int kEnergyPoints = 50;
        static const unsigned int kAttackDamage = 20;

        ScavTrap(void);
        explicit ScavTrap(std::string const &name);
        ScavTrap(ScavTrap const &other);
        ~ScavTrap(void);
        ScavTrap &operator=(ScavTrap const &other);

        // ClapTrap::attack を「隠蔽」する。virtual ではないため、呼ばれる実装は
        // 変数の静的な型で決まる。ClapTrap 経由で呼ぶと ClapTrap::attack が動く。
        void attack(const std::string &target);

        void guardGate(void);
};

#endif
