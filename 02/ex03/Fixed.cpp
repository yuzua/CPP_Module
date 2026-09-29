#include <cmath>
#include "Fixed.hpp"

const int Fixed::_fractionalBits;

Fixed::Fixed(void) : _value(0) {}

Fixed::Fixed(int const value) : _value(value * (1 << _fractionalBits)) {}

Fixed::Fixed(float const value)
    : _value(static_cast<int>(roundf(value * static_cast<float>(1 << _fractionalBits)))) {}

Fixed::Fixed(Fixed const &other) : _value(other._value) {}

Fixed &Fixed::operator=(Fixed const &rhs) {
    if (this != &rhs)
        this->_value = rhs.getRawBits();
    return *this;
}

Fixed::~Fixed(void) {}

bool Fixed::operator<(Fixed const &rhs) const {
    return this->_value < rhs._value;
}

bool Fixed::operator==(Fixed const &rhs) const {
    return this->_value == rhs._value;
}

bool Fixed::operator>(Fixed const &rhs) const {
    return rhs < *this;
}

bool Fixed::operator<=(Fixed const &rhs) const {
    return !(rhs < *this);
}

bool Fixed::operator>=(Fixed const &rhs) const {
    return !(*this < rhs);
}

bool Fixed::operator!=(Fixed const &rhs) const {
    return !(*this == rhs);
}

Fixed Fixed::operator+(Fixed const &rhs) const {
    Fixed result;

    result.setRawBits(this->_value + rhs._value);
    return result;
}

Fixed Fixed::operator-(Fixed const &rhs) const {
    Fixed result;

    result.setRawBits(this->_value - rhs._value);
    return result;
}

Fixed Fixed::operator*(Fixed const &rhs) const {
    long const product = static_cast<long>(this->_value) * static_cast<long>(rhs._value);
    Fixed result;

    result.setRawBits(static_cast<int>(product / (1 << _fractionalBits)));
    return result;
}

Fixed Fixed::operator/(Fixed const &rhs) const {
    long const dividend = static_cast<long>(this->_value) * (1 << _fractionalBits);
    Fixed result;

    result.setRawBits(static_cast<int>(dividend / rhs._value));
    return result;
}

Fixed &Fixed::operator++(void) {
    ++this->_value;
    return *this;
}

Fixed &Fixed::operator--(void) {
    --this->_value;
    return *this;
}

Fixed Fixed::operator++(int) {
    Fixed previous(*this);

    ++(*this);
    return previous;
}

Fixed Fixed::operator--(int) {
    Fixed previous(*this);

    --(*this);
    return previous;
}

Fixed &Fixed::min(Fixed &a, Fixed &b) {
    return b < a ? b : a;
}

Fixed const &Fixed::min(Fixed const &a, Fixed const &b) {
    return b < a ? b : a;
}

Fixed &Fixed::max(Fixed &a, Fixed &b) {
    return a < b ? b : a;
}

Fixed const &Fixed::max(Fixed const &a, Fixed const &b) {
    return a < b ? b : a;
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
