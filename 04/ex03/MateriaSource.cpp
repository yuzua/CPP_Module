#include "MateriaSource.hpp"

#include "AMateria.hpp"

int const MateriaSource::kSlotCount;

MateriaSource::MateriaSource(void) {
    this->nullTemplates();
}

MateriaSource::MateriaSource(MateriaSource const &other) {
    this->nullTemplates();
    this->cloneTemplatesFrom(other);
}

MateriaSource::~MateriaSource(void) {
    this->deleteTemplates();
}

MateriaSource &MateriaSource::operator=(MateriaSource const &other) {
    if (this != &other) {
        this->deleteTemplates();
        this->cloneTemplatesFrom(other);
    }
    return *this;
}

void MateriaSource::learnMateria(AMateria *materia) {
    int i;

    if (materia == 0)
        return;
    for (i = 0; i < kSlotCount; ++i) {
        if (this->templates_[i] == materia)
            return;
    }
    for (i = 0; i < kSlotCount; ++i) {
        if (this->templates_[i] == 0) {
            this->templates_[i] = materia;
            return;
        }
    }
    delete materia;
}

AMateria *MateriaSource::createMateria(std::string const &type) {
    for (int i = 0; i < kSlotCount; ++i) {
        if (this->templates_[i] != 0 && this->templates_[i]->getType() == type)
            return this->templates_[i]->clone();
    }
    return 0;
}

void MateriaSource::nullTemplates(void) {
    for (int i = 0; i < kSlotCount; ++i)
        this->templates_[i] = 0;
}

void MateriaSource::deleteTemplates(void) {
    for (int i = 0; i < kSlotCount; ++i) {
        delete this->templates_[i];
        this->templates_[i] = 0;
    }
}

void MateriaSource::cloneTemplatesFrom(MateriaSource const &other) {
    for (int i = 0; i < kSlotCount; ++i) {
        if (other.templates_[i] != 0)
            this->templates_[i] = other.templates_[i]->clone();
    }
}
