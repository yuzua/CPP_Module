#ifndef WRONGANIMAL_HPP
#define WRONGANIMAL_HPP

#include <string>

class WrongAnimal {
    protected:
        std::string type_;
    public:
        WrongAnimal();
        WrongAnimal(const std::string &type);
        WrongAnimal(const WrongAnimal &other);
        virtual ~WrongAnimal();
        WrongAnimal &operator=(const WrongAnimal &other);
        std::string getType() const;
        void makeSound() const;
};

#endif