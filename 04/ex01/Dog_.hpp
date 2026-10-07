#ifndef DOG_HPP
#define DOG_HPP

#include "Animal.hpp"

#include <string>

class Brain;

// INV3  生存中、brain_ はちょうど 1 個の Brain を指す。
// INV4  その Brain を delete するのは ~Dog だけ。
class Dog : public Animal {
    public:
        Dog(void);
        Dog(Dog const &other);
        virtual ~Dog(void);
        Dog &operator=(Dog const &other);

        virtual void       makeSound(void) const;
        void               setIdea(int index, std::string const &idea);
        std::string const &getIdea(int index) const;

    private:
        Brain *brain_;
};

#endif
