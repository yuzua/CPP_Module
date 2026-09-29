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
        int getRawBits(void) const;
        void setRawBits(int const raw);
        float toFloat( void ) const;
        int toInt( void ) const;
};

std::ostream &operator<<(std::ostream &os, Fixed const &value);

# endif