#ifndef BIGNUM_HPP
#define BIGNUM_HPP

#include <vector>
#include <string>

class Bignum
{
public:
    Bignum();
    Bignum(const std::string &str);
    std::vector<int> value;

    Bignum operator+(const Bignum &other) const;
    Bignum operator-(const Bignum &other) const;
    Bignum operator*(const Bignum &other) const;
    Bignum operator/(const Bignum &other) const;
    Bignum operator%(const Bignum &other) const;
    Bignum operator^(std::pair<Bignum, Bignum> exp_mod) const;

    bool operator==(const Bignum &other) const;
    bool operator<(const Bignum &other) const;
    bool operator>(const Bignum &other) const;
    bool operator<=(const Bignum &other) const;
    bool operator>=(const Bignum &other) const;
    std::string to_string() const;

private:
    void check() const;
};

#endif
