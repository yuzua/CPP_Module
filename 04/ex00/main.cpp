#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"

#include <iostream>

int main(void) {
    std::cout << "各動物のmakeSoundが呼ばれることの確認" << std::endl;
    const Animal *meta = new Animal();
    const Animal *j = new Dog();
    const Animal *i = new Cat();

    std::cout << j->getType() << std::endl;
    std::cout << i->getType() << std::endl;
    i->makeSound();
    j->makeSound();
    meta->makeSound();

    delete i;
    delete j;
    delete meta;

    std::cout << "virtualがないとWrongAnimalのmakeSoundが呼ばれることの確認" << std::endl;
    const WrongAnimal *wrongMeta = new WrongCat();

    std::cout << wrongMeta->getType() << std::endl;
    wrongMeta->makeSound();

    delete wrongMeta;

    std::cout << "ポインタでなければWrongCatのmakeSoundが呼ばれることの確認" << std::endl;
    WrongCat direct;

    direct.makeSound();

    return 0;
}