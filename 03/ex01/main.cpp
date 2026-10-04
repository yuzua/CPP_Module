#include "ScavTrap.hpp"

#include <iostream>

int main() {
    std::string test = "TEST";

    std::cout << std::endl << "--- ScavTrapの基本ケース ---" << std::endl;
    // ClapTrapが先に作成され、消すときはScavTrapが先に実行されることの確認
    ScavTrap* hoge = new ScavTrap("HOGE");
    hoge->attack(test);
    hoge->takeDamage(30);
    hoge->beRepaired(10);
    hoge->guardGate();
    delete hoge;

    std::cout << std::endl << "--- スコープを抜けたときの破棄順 ---" << std::endl;
    {
        ScavTrap hoge2("HOGE2");
    }

    std::cout << std::endl << "--- ClapTrapとして呼ぶと親のattackになる ---" << std::endl;
    ScavTrap* hoge3 = new ScavTrap("HOGE3");
    ClapTrap* asClapTrap = hoge3;
    hoge3->attack(test);
    asClapTrap->attack(test);
    delete hoge3;

    std::cout << std::endl << "--- 倒れたあとの異常系確認 ---" << std::endl;
    ScavTrap* hoge4 = new ScavTrap("HOGE4");
    hoge4->takeDamage(100);
    hoge4->attack(test);
    hoge4->beRepaired(1);
    delete hoge4;

    std::cout << std::endl << "--- 攻撃しすぎの異常系確認 ---" << std::endl;
    ScavTrap* hoge5 = new ScavTrap("HOGE5");
    for (int i = 0; i < 50; ++i)
        hoge5->attack(test);
    hoge5->attack(test);
    hoge5->beRepaired(1);
    delete hoge5;

    return 0;
}
