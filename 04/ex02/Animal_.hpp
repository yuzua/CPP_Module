#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <string>

// 動物の共通の窓口。
//
// INV1  構築が終わったときの type は、コンストラクタが受け取った種類名である。
// INV2  makeSound() は状態を変えない。
//
// type は課題指定どおり protected。書き込みはコンストラクタに寄せ、
// 派生クラスの本体では代入しない。
class Animal {
    public:
        Animal(void);
        Animal(Animal const &other);
        virtual ~Animal(void);
        Animal &operator=(Animal const &other);

        std::string const &getType(void) const;
        // 純粋仮想。Animal は抽象クラスになり、new Animal() も Animal の値も作れない。
        virtual void makeSound(void) const = 0;

    protected:
        explicit Animal(std::string const &type);
        std::string type;
};

#endif
