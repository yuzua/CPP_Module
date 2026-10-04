#include "FragTrap.hpp"

#include <iostream>

int main() {
    std::string test = "TEST";

    std::cout << std::endl << "--- FragTrapの基本ケース ---" << std::endl;
    // ClapTrapが先に作成され、消すときはFragTrapが先に実行されることの確認
    FragTrap* hoge = new FragTrap("HOGE");
    hoge->attack(test);
    hoge->takeDamage(30);
    hoge->beRepaired(10);
    hoge->highFivesGuys();
    delete hoge;

    std::cout << std::endl << "--- スコープを抜けたときの破棄順 ---" << std::endl;
    {
        FragTrap hoge2("HOGE2");
    }

    std::cout << std::endl << "--- 倒れたあとの異常系確認 ---" << std::endl;
    FragTrap* hoge3 = new FragTrap("HOGE3");
    hoge3->takeDamage(100);
    hoge3->attack(test);
    hoge3->beRepaired(1);
    delete hoge3;

    std::cout << std::endl << "--- 攻撃しすぎの異常系確認 ---" << std::endl;
    FragTrap* hoge4 = new FragTrap("HOGE4");
    for (int i = 0; i < 100; ++i)
        hoge4->attack(test);
    hoge4->attack(test);
    hoge4->beRepaired(1);
    delete hoge4;

    return 0;
}
