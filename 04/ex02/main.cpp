#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"

#include <iostream>

int main(void) {
    std::cout << "Animal*として1匹ずつdeleteする" << std::endl;
    const Animal *j = new Dog();
    const Animal *i = new Cat();

    delete j;
    delete i;

    std::cout << "半分がDog、半分がCatの配列をAnimal*としてdeleteする" << std::endl;
    const int count = 4;
    Animal *animals[4];

    for (int n = 0; n < count; ++n) {
        if (n < count / 2)
            animals[n] = new Dog();
        else
            animals[n] = new Cat();
    }
    for (int n = 0; n < count; ++n)
        delete animals[n];

    std::cout << "Dogのコピーが別のBrainを持つか" << std::endl;
    Dog original_dog;

    original_dog.setIdea(0, "chase");

    Dog copy_dog(original_dog);

    std::cout << original_dog.getIdea(0) << std::endl;
    std::cout << copy_dog.getIdea(0) << std::endl;
    copy_dog.setIdea(0, "sleep");
    std::cout << original_dog.getIdea(0) << std::endl;
    std::cout << copy_dog.getIdea(0) << std::endl;

    Dog assigned_dog;

    assigned_dog = original_dog;
    std::cout << assigned_dog.getIdea(0) << std::endl;
    assigned_dog.setIdea(0, "eat");
    std::cout << original_dog.getIdea(0) << std::endl;
    std::cout << assigned_dog.getIdea(0) << std::endl;

    std::cout << "Catのコピーが別のBrainを持つか" << std::endl;
    Cat original_cat;

    original_cat.setIdea(0, "nap");

    Cat copy_cat(original_cat);

    copy_cat.setIdea(0, "play");
    std::cout << original_cat.getIdea(0) << std::endl;
    std::cout << copy_cat.getIdea(0) << std::endl;

    return 0;
}