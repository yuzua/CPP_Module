#ifndef ICE_HPP
#define ICE_HPP

#include "AMateria.hpp"

class Ice : public AMateria {
    public:
        Ice(void);
        Ice(Ice const &other);
        virtual ~Ice(void);
        Ice &operator=(Ice const &other);

        virtual AMateria *clone(void) const;
        virtual void      use(ICharacter &target);
};

#endif
