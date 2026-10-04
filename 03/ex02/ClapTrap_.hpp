#ifndef CLAPTRAP_HPP
#define CLAPTRAP_HPP

#include <string>

// 戦闘リソース（ヒットポイント / エナジーポイント）を持つロボット。
//
// 不変条件
//   INV1 hitPoints_  は 0 以上。0 は「破壊済み」を意味する。
//   INV2 energyPoints_ は 0 以上。0 は「行動不能」を意味する。
//   INV3 戦闘操作は name_ / attackDamage_ を変えない。代入だけが identity ごと置き換える。
//   INV4 行動（attack / beRepaired）が成功したとき、エナジーはちょうど 1 減る。
//   INV5 破壊済み（hitPoints_ == 0）からは回復できない。吸収状態である。
//
// 上記を全ての書き込み経路で保証するため、属性は private に閉じている。
// 派生クラスは protected コンストラクタで初期値を渡すことしかできず、
// 構築後の書き換えは tryBeginAction() / takeDamage() / beRepaired() を経由する。
class ClapTrap {
    public:
        static const unsigned int kBaseHitPoints    = 10;
        static const unsigned int kBaseEnergyPoints = 10;
        static const unsigned int kBaseAttackDamage = 0;
        static char const * const kDefaultName;

        ClapTrap(void);
        explicit ClapTrap(std::string const &name);
        ClapTrap(ClapTrap const &other);
        ~ClapTrap(void);
        ClapTrap &operator=(ClapTrap const &other);

        void attack(const std::string &target);
        void takeDamage(unsigned int amount);
        void beRepaired(unsigned int amount);

        // 観測専用。呼び出し側が状態を検証できるようにするためのもので、
        // 書き込み経路は公開しない。
        std::string const &getKind(void) const;
        std::string const &getName(void) const;
        unsigned int getHitPoints(void) const;
        unsigned int getEnergyPoints(void) const;
        unsigned int getAttackDamage(void) const;

    protected:
        // 派生クラス専用の初期化経路。属性を「初期化」でしか決められないため、
        // 構築直後から不変条件が成立する。kind は実体の種類を表す表示名。
        ClapTrap(std::string const &kind, std::string const &name,
                 unsigned int hitPoints, unsigned int energyPoints,
                 unsigned int attackDamage);

        // 行動の可否を判定し、可能ならエナジーを 1 消費して true を返す。
        // 行動できない場合は理由を出力し、状態を変更せずに false を返す。
        bool tryBeginAction(void);

    private:
        std::string  kind_;
        std::string  name_;
        unsigned int hitPoints_;
        unsigned int energyPoints_;
        unsigned int attackDamage_;

        static unsigned int subtractClamped(unsigned int value, unsigned int amount);
        static unsigned int addClamped(unsigned int value, unsigned int amount);
};

#endif
