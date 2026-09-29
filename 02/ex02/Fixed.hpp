#ifndef FIXED_HPP
#define FIXED_HPP


class Fixed {
    private:
        int value_;
        static const int kFractionalBit;
    public:
        Fixed();
        Fixed(int const value);
        Fixed(float const value);
        Fixed(const Fixed &other);
        ~Fixed();
        Fixed &operator=(const Fixed &other);
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
        static Fixed &min(Fixed &a, Fixed &b);
        static Fixed const &min(Fixed const &a, Fixed const &b);
        static Fixed &max(Fixed &a, Fixed &b);
        static Fixed const &max(Fixed const &a, Fixed const &b);
        int getRawBits(void) const;
        void setRawBits(int const raw);
        float toFloat( void ) const;
        int toInt( void ) const;
};

std::ostream &operator<<(std::ostream &os, Fixed const &value);

# endif