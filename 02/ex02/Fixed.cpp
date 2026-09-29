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

bool Fixed::operator<(Fixed const &rhs) const {
    return value_ < rhs.value_;
}

bool Fixed::operator==(Fixed const &rhs) const {
    return value_ == rhs.value_;
}

bool Fixed::operator>(Fixed const &rhs) const {
    return value_ > rhs.value_;
}

bool Fixed::operator<=(Fixed const &rhs) const {
    return value_ <= rhs.value_;
}

bool Fixed::operator>=(Fixed const &rhs) const {
    return value_ >= rhs.value_;
}

bool Fixed::operator!=(Fixed const &rhs) const {
    return value_ != rhs.value_;
}

Fixed Fixed::operator+(Fixed const &rhs) const {
    Fixed fixed;
    fixed.setRawBits(getRawBits() + rhs.getRawBits());
    return fixed;
}

Fixed Fixed::operator-(Fixed const &rhs) const {
    Fixed fixed;
    fixed.setRawBits(getRawBits() - rhs.getRawBits());
    return fixed;
}

Fixed Fixed::operator*(Fixed const &rhs) const {
    Fixed fixed;
    fixed.setRawBits(getRawBits() * rhs.getRawBits());
    return fixed;
}

Fixed Fixed::operator/(Fixed const &rhs) const {
    Fixed fixed;
    fixed.setRawBits(getRawBits() / rhs.getRawBits());
    return fixed;
}

Fixed &Fixed::operator++(void) {
    setRawBits(getRawBits() + (1 << kFractionalBit));
    return *this;
}

Fixed &Fixed::operator--(void) {
    setRawBits(getRawBits() - (1 << kFractionalBit));
    return *this;
}

Fixed Fixed::operator++(int) {
    Fixed fixed(*this);
    setRawBits(getRawBits() + (1 << kFractionalBit));
    return fixed;
}

Fixed Fixed::operator--(int) {
    Fixed fixed(*this);
    setRawBits(getRawBits() - (1 << kFractionalBit));
    return fixed;
}

Fixed &Fixed::min(Fixed &a, Fixed &b) {
    return a < b ? a : b;
}

Fixed &Fixed::max(Fixed &a, Fixed &b) {
    return a > b ? a : b;
}

Fixed const &Fixed::min(Fixed const &a, Fixed const &b) {
    return a < b ? a : b;
}

Fixed const &Fixed::max(Fixed const &a, Fixed const &b) {
    return a > b ? a : b;
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
