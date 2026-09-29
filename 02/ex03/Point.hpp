#ifndef POINT_HPP
#define POINT_HPP

#include "Fixed.hpp"

// 不変（immutable）な 2D 座標。_x / _y が const なので生成後は変更できない。
class Point {
private:
    Fixed const _x;
    Fixed const _y;

public:
    Point(void);
    Point(float const x, float const y);
    Point(Point const &other);

    // _x / _y が const のため値を変更できない。
    // 正統的正準形式の要求を満たすために定義だけ存在する、意図的な no-op。
    Point &operator=(Point const &rhs);

    ~Point(void);

    Fixed const &getX(void) const;
    Fixed const &getY(void) const;
};

// 点が三角形 abc の内側にあれば true。頂点上・辺上は false。
bool bsp(Point const a, Point const b, Point const c, Point const point);

#endif
