#ifndef MATERIASOURCE_HPP
#define MATERIASOURCE_HPP

#include "IMateriaSource.hpp"

class AMateria;

// 見本スロットは空か、他と共有しないマテリア 1 個（INV7）。
// createMateria は見本の clone を返し、見本そのものは返さない。
class MateriaSource : public IMateriaSource {
    public:
        MateriaSource(void);
        MateriaSource(MateriaSource const &other);
        virtual ~MateriaSource(void);
        MateriaSource &operator=(MateriaSource const &other);

        virtual void      learnMateria(AMateria *materia);
        virtual AMateria *createMateria(std::string const &type);

    private:
        static int const kSlotCount = 4;

        AMateria *templates_[kSlotCount];

        void nullTemplates(void);
        void deleteTemplates(void);
        void cloneTemplatesFrom(MateriaSource const &other);
};

#endif
