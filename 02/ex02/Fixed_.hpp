#ifndef FIXED_HPP
#define FIXED_HPP

#include <iostream>

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

    // 比較: < と == が意味の正本。残り 4 つはそこから導く。
    bool operator<(Fixed const &rhs) const;
    bool operator==(Fixed const &rhs) const;
    bool operator>(Fixed const &rhs) const;
    bool operator<=(Fixed const &rhs) const;
    bool operator>=(Fixed const &rhs) const;
    bool operator!=(Fixed const &rhs) const;

    // 算術: 生値のまま整数演算する。+ と - は誤差ゼロ。
    Fixed operator+(Fixed const &rhs) const;
    Fixed operator-(Fixed const &rhs) const;
    Fixed operator*(Fixed const &rhs) const;
    Fixed operator/(Fixed const &rhs) const;   // 0 除算はクラッシュする（subject 許容）

    // 増減: 生値を 1 (= 2^-8) だけ動かす。
    Fixed &operator++(void);
    Fixed &operator--(void);
    Fixed operator++(int);
    Fixed operator--(int);

    // 同値のときは a を返す。
    static Fixed       &min(Fixed &a, Fixed &b);
    static Fixed const &min(Fixed const &a, Fixed const &b);
    static Fixed       &max(Fixed &a, Fixed &b);
    static Fixed const &max(Fixed const &a, Fixed const &b);

    int   getRawBits(void) const;
    void  setRawBits(int const raw);
    float toFloat(void) const;
    int   toInt(void) const;   // 負方向に丸める（floor）
};

std::ostream &operator<<(std::ostream &os, Fixed const &value);

#endif
