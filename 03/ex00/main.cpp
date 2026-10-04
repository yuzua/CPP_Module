#include "ClapTrap.hpp"

#include <iostream>

int main() {
    std::string dummy = "ダミー";
    std::string slime = "スライム";

    std::cout << std::endl << "--- ClapTrapの基本ケース ---" << std::endl;
    ClapTrap* bob = new ClapTrap("Bob");
    bob->attack(slime);
    bob->takeDamage(5);
    bob->beRepaired(3);
    delete bob;

    std::cout << std::endl << "--- deleteのメッセージが出るか確認 ---" << std::endl;
    {
        ClapTrap alice("Alice");
    }

    std::cout << std::endl << "--- 修理は指定した量だけ戻るか確認 ---" << std::endl;
    ClapTrap* medic = new ClapTrap("Medic");
    medic->takeDamage(2);
    medic->beRepaired(50);
    delete medic;

    std::cout << std::endl << "--- ダメージが大きすぎる場合の異常系確認 ---" << std::endl;
    ClapTrap* glass = new ClapTrap("Glass");
    glass->takeDamage(9999);
    glass->takeDamage(1);
    delete glass;

    std::cout << std::endl << "--- 倒れたあとの異常系確認 ---" << std::endl;
    ClapTrap* ghost = new ClapTrap("Ghost");
    ghost->takeDamage(10);
    ghost->attack(dummy);
    ghost->beRepaired(1);
    delete ghost;

    std::cout << std::endl << "--- 攻撃しすぎの異常系確認 ---" << std::endl;
    ClapTrap* spammer = new ClapTrap("Spammer");
    for (int i = 0; i < 10; ++i)
        spammer->attack(slime);
    spammer->attack(slime);
    spammer->beRepaired(1);
    delete spammer;

    return 0;
}
