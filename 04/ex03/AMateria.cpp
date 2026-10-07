#include "AMateria.hpp"

#include "ICharacter.hpp"

// 初期化子リストの type(type) は、メンバ type を引数 type で初期化する。
// 本体で `type = type` と書くと、引数自身への代入になり、メンバは空のまま残る。
AMateria::AMateria(void) : type("") {}

AMateria::AMateria(std::string const &type) : type(type) {}

AMateria::AMateria(AMateria const &other) : type(other.type) {}

AMateria::~AMateria(void) {}

AMateria &AMateria::operator=(AMateria const &) {
    return *this;
}

std::string const &AMateria::getType(void) const {
    return this->type;
}

void AMateria::use(ICharacter &) {}
