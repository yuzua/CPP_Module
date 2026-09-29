#include "Point.hpp"

// const メンバは「初期化」はできるが「代入」はできない。
// だからコンストラクタは初期化リストで書ける。
Point::Point(void) : _x(0), _y(0) {}

Point::Point(float const x, float const y) : _x(x), _y(y) {}

Point::Point(Point const &other) : _x(other._x), _y(other._y) {}

Point &Point::operator=(Point const &rhs) {
    (void)rhs;
    return *this;
}

Point::~Point(void) {}

Fixed const &Point::getX(void) const {
    return this->_x;
}

Fixed const &Point::getY(void) const {
    return this->_y;
}
