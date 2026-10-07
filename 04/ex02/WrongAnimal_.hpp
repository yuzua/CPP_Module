#ifndef WRONGANIMAL_HPP
#define WRONGANIMAL_HPP

#include <string>

// Animal と同じ形に見える比較用。
// makeSound は意図的に virtual ではない。名前隠蔽になり、基底ポインタからは
// WrongAnimal の本体が呼ばれる。
// デストラクタは virtual にする。鳴き声の実験のために delete を未定義動作にしない。
class WrongAnimal {
    public:
        WrongAnimal(void);
        WrongAnimal(WrongAnimal const &other);
        virtual ~WrongAnimal(void);
        WrongAnimal &operator=(WrongAnimal const &other);

        std::string const &getType(void) const;
        void makeSound(void) const;

    protected:
        explicit WrongAnimal(std::string const &type);
        std::string type;
};

#endif
