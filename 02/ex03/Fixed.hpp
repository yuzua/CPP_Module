#ifndef FIXED_HPP
#define FIXED_HPP

#include <iostream>

// ex02 の Fixed と同一。ただし bsp のテスト出力が埋もれないよう、
// コンストラクタ / デストラクタのログは出さない（ex03 は出力仕様が無いため）。
class Fixed {
private:
    int                 _value;
    static const int    _fractionalBits = 8;

public:
    Fixed(void);
    Fixed(int const value);
    Fixed(float const value);
    Fixed(Fixed const &other);
    Fixed &operator=(Fixed const &rhs);
    ~Fixed(void);

    bool operator<(Fixed const &rhs) const;
    bool operator==(Fixed const &rhs) const;
    bool operator>(Fixed const &rhs) const;
    bool operator<=(Fixed const &rhs) const;
    bool operator>=(Fixed const &rhs) const;
    bool operator!=(Fixed const &rhs) const;

    Fixed operator+(Fixed const &rhs) const;
    Fixed operator-(Fixed const &rhs) const;
    Fixed operator*(Fixed const &rhs) const;
    Fixed operator/(Fixed const &rhs) const;

    Fixed &operator++(void);
    Fixed &operator--(void);
    Fixed operator++(int);
    Fixed operator--(int);

    static Fixed       &min(Fixed &a, Fixed &b);
    static Fixed const &min(Fixed const &a, Fixed const &b);
    static Fixed       &max(Fixed &a, Fixed &b);
    static Fixed const &max(Fixed const &a, Fixed const &b);

    int   getRawBits(void) const;
    void  setRawBits(int const raw);
    float toFloat(void) const;
    int   toInt(void) const;
};

std::ostream &operator<<(std::ostream &os, Fixed const &value);

#endif
