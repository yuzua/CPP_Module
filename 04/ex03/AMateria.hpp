#ifndef AMATERIA_HPP
#define AMATERIA_HPP

#include <string>

class ICharacter;

// INV5  type は構築時に決まり、代入では変わらない。
// clone() が純粋仮想なので、このクラス自体はインスタンス化できない。
class AMateria {
    public:
        AMateria(void);
        explicit AMateria(std::string const &type);
        AMateria(AMateria const &other);
        virtual ~AMateria(void);
        AMateria &operator=(AMateria const &other);

        std::string const &getType(void) const;
        virtual AMateria  *clone(void) const = 0;
        virtual void       use(ICharacter &target);

    protected:
        std::string type;
};

#endif
