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

// --- 比較 ----------------------------------------------------------------
// 両辺とも同じ倍率 2^8 なので、生値をそのまま比べれば大小関係は一致する。

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

// --- 算術 ----------------------------------------------------------------

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

// (a/2^8) * (b/2^8) = (a*b)/2^16 なので、生値は a*b を 2^8 で割り戻す。
// a*b は最大 62bit 必要なので long で受ける（C++98 に long long は無い）。
Fixed Fixed::operator*(Fixed const &rhs) const {
    long const product = static_cast<long>(this->_value) * static_cast<long>(rhs._value);
    Fixed result;

    result.setRawBits(static_cast<int>(product / (1 << _fractionalBits)));
    return result;
}

// (a/2^8) / (b/2^8) = a/b なので、生値は a を先に 2^8 倍してから b で割る。
// 順序を逆にすると小数部が消える。
Fixed Fixed::operator/(Fixed const &rhs) const {
    long const dividend = static_cast<long>(this->_value) * (1 << _fractionalBits);
    Fixed result;

    result.setRawBits(static_cast<int>(dividend / rhs._value));
    return result;
}

// --- 増減 ----------------------------------------------------------------
// 1 + e > 1 となる最小の e は生値 1、つまり 2^-8 = 0.00390625。

Fixed &Fixed::operator++(void) {
    ++this->_value;
    return *this;
}

Fixed &Fixed::operator--(void) {
    --this->_value;
    return *this;
}

// 引数の int は前置と区別するためのダミーなので名前を付けない（未使用警告対策）。
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

// --- min / max -----------------------------------------------------------
// const 版が無いと Fixed const を渡せない。非 const 版が無いと結果を書き換えられない。

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

// --- 変換 ----------------------------------------------------------------

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
