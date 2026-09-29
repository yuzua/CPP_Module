#include <cmath>
#include "Fixed.hpp"

const int Fixed::_fractionalBits;

Fixed::Fixed(void) : _value(0) {
    std::cout << "Default constructor called" << std::endl;
}

// 負の値の左シフトは C++98 で未定義動作のため、乗算で桁を上げる。
Fixed::Fixed(int const value) : _value(value * (1 << _fractionalBits)) {
    std::cout << "Int constructor called" << std::endl;
}

// roundf は <cmath> 経由でも std:: に入らない場合があるため無修飾で呼ぶ。
Fixed::Fixed(float const value)
    : _value(static_cast<int>(roundf(value * static_cast<float>(1 << _fractionalBits)))) {
    std::cout << "Float constructor called" << std::endl;
}

Fixed::Fixed(Fixed const &other) : _value(0) {
    std::cout << "Copy constructor called" << std::endl;
    *this = other;
}

Fixed &Fixed::operator=(Fixed const &rhs) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &rhs)
        this->_value = rhs.getRawBits();
    return *this;
}

Fixed::~Fixed(void) {
    std::cout << "Destructor called" << std::endl;
}

int Fixed::getRawBits(void) const {
    return this->_value;
}

void Fixed::setRawBits(int const raw) {
    this->_value = raw;
}

float Fixed::toFloat(void) const {
    return static_cast<float>(this->_value) / static_cast<float>(1 << _fractionalBits);
}

int Fixed::toInt(void) const {
    return this->_value >> _fractionalBits;
}

std::ostream &operator<<(std::ostream &os, Fixed const &value) {
    os << value.toFloat();
    return os;
}
