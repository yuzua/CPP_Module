#include <iostream>
#include <cmath>

#include "Fixed.hpp"

const int Fixed::kFractionalBit = 8;

Fixed::Fixed(void) : value_(0) {
    std::cout << "Default constructor called" << std::endl;
}

Fixed::Fixed(int const value) : value_(value * (1 << kFractionalBit)) {
    std::cout << "Int constructor called" << std::endl;
}

Fixed::Fixed(float const value) : value_(static_cast<int>(roundf(value * (1 << kFractionalBit)))) {
    std::cout << "Float constructor called" << std::endl;
}

Fixed::Fixed(const Fixed &other) : value_(0) {
    std::cout << "Copy constructor called" << std::endl;
    *this = other;
}

Fixed::~Fixed() {
    std::cout << "Destructor called" << std::endl;
}

Fixed &Fixed::operator=(const Fixed &other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other) {
        setRawBits(other.getRawBits());
    }
    return *this;
}

int Fixed::getRawBits(void) const {
    return value_;
}

void Fixed::setRawBits(int const raw) {
    value_ = raw;
}

float Fixed::toFloat(void) const {
    return static_cast<float>(value_) / (1 << kFractionalBit);
}

int Fixed::toInt(void) const {
    return value_ / (1 << kFractionalBit);
}

std::ostream &operator<<(std::ostream &os, Fixed const &value) {
    os << value.toFloat();
    return os;
}
