#ifndef FIXED_HPP
#define FIXED_HPP


class Fixed {
    private:
        int value_;
        static const int kFractionalBit;
    public:
        Fixed();
        Fixed(const Fixed &other);
        ~Fixed();
        Fixed &operator=(const Fixed &other);
        int getRawBits(void) const;
        void setRawBits(int const raw);
};

# endif