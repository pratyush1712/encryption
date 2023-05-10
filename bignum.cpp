#include "bignum.hpp"

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <map>

Bignum::Bignum()
{
    value.push_back(0);
    // check();
}

Bignum::Bignum(const std::string &str)
{
    for (char c : str)
    {
        value.push_back(c - '0');
    }
    std::reverse(value.begin(), value.end());
    if (value.empty())
    {
        value.push_back(0);
    }
    // check();
}

void Bignum::check() const
{
    if (value.empty())
    {
        throw std::invalid_argument("Bignum has no digits.");
    }
    if (value.back() == 0 && value.size() > 1)
    {
        throw std::invalid_argument("Bignum has leading zeroes.");
    }
    for (int digit : value)
    {
        if (digit < 0 || digit > 9)
        {
            throw std::invalid_argument("Bignum has invalid digit.");
        }
    }
}

Bignum Bignum::operator+(const Bignum &other) const
{
    Bignum result;
    int carried = 0;
    result.value.clear();
    for (unsigned int i = 0; i < std::max(value.size(), other.value.size()); i++)
    {
        int digit = carried;
        if (i < value.size())
        {
            digit += value[i];
        }
        if (i < other.value.size())
        {
            digit += other.value[i];
        }
        result.value.push_back(digit % 10);
        carried = digit / 10;
    }
    if (carried > 0)
    {
        result.value.push_back(carried);
    }
    return result;
}

Bignum Bignum::operator-(const Bignum &other) const
{
    Bignum result;
    int carried = 0;
    if (other.value.size() > value.size())
    {
        std::cout << "Unsupported: Negative number" << std::endl;
        return Bignum();
    }

    result.value.clear();

    for (unsigned int i = 0; i < value.size(); i++)
    {
        int digit = value[i] + carried;
        if (i < other.value.size())
        {
            if (digit < other.value[i])
            {
                digit += 10;
                carried = -1;
            }
            else
            {
                carried = 0;
            }
            digit -= other.value[i];
        }
        else if (carried)
        {
            carried = 0;
        }
        result.value.push_back(digit);
    }
    if (carried == -1)
    {
        std::cout << "Unsupported: Negative number" << std::endl;
        return Bignum();
    }
    // clear all leading 0s
    while (result.value.size() > 1 && result.value.back() == 0)
    {
        result.value.pop_back();
    }
    return result;
}

// Bignum Bignum::multi_helper(int digit) const
// {
//     Bignum result;
//     result.value.resize(value.size(), 0);
//     int carry = 0;
//     for (unsigned i = 0; i < value.size(); i++)
//     {
//         int sum = value[i] * digit + carry;
//         result.value[i] = sum % 10;
//         carry = sum / 10;
//     }
//     if (carry > 0)
//     {
//         result.value.push_back(carry);
//     }
//     // remove leading 0s
//     while (result.value.size() > 1 && result.value.back() == 0)
//     {
//         result.value.pop_back();
//     }
//     return result;
// }

// Bignum Bignum::operator*(const Bignum &other) const
// {
//     // first check if either operand is 0
//     if ((value.size() == 1 && value[0] == 0) || (other.value.size() == 1 && other.value[0] == 0))
//     {
//         return Bignum();
//     }
//     const Bignum *larger = std::max(this, &other);
//     const Bignum &smaller = larger == this ? other : *this;
//     // calculate and store all multiples of smaller from 1 to 9 in a map
//     std::map<int, Bignum> multiples;
//     for (int i = 0; i <= 9; i++)
//     {
//         multiples[i] = smaller.multi_helper(i);
//     }
//     // now multiply larger by each digit in smaller
//     Bignum result;
//     Bignum carry("");
//     for (unsigned i = 0; i < larger->value.size(); i++)
//     {
//         int digit = larger->value[i];
//         Bignum temp = multiples[digit] + carry;
//         // temp's first digit is the result digit, the rest are carry
//         result.value.push_back(temp.value[0]);
//         carry.value = std::vector<int>(temp.value.begin() + 1, temp.value.end());
//     }
//     // add the last carry
//     while (carry.value.size() > 0)
//     {
//         result.value.push_back(carry.value[0]);
//         carry.value.erase(carry.value.begin());
//     }
//     // remove leading 0s
//     while (result.value.size() > 1 && result.value.back() == 0)
//     {
//         result.value.pop_back();
//     }
//     // remove one zero from the front
//     if (result.value.size() > 1 && result.value[0] == 0)
//     {
//         result.value.erase(result.value.begin());
//     }
//     return result;
// }

Bignum Bignum::operator*(const Bignum &other) const
{
    Bignum result;
    result.value.resize(value.size() + other.value.size(), 0);
    for (unsigned i = 0; i < value.size(); i++)
    {
        int carry = 0;
        for (unsigned j = 0; j < other.value.size() || carry > 0; j++)
        {
            int sum = result.value[i + j] + carry;
            if (j < other.value.size())
            {
                sum += value[i] * other.value[j];
            }
            result.value[i + j] = sum % 10;
            carry = sum / 10;
        }
    }
    // clear all leading 0s
    while (result.value.size() > 1 && result.value.back() == 0)
    {
        result.value.pop_back();
    }
    return result;
}

std::pair<Bignum, Bignum> Bignum::operator/(const Bignum &divisor) const
{
    if (divisor.value.size() == 1 && divisor.value[0] == 0)
    {
        std::cout << "Error: Divide by zero." << std::endl;
        return std::make_pair(Bignum(), Bignum());
    }
    Bignum current;
    Bignum result;
    for (int i = this->value.size() - 1; i >= 0; i--)
    {
        current.value.insert(current.value.begin(), this->value[i]);
        while (current.value.size() > 1 && current.value.back() == 0)
        {
            current.value.pop_back();
        }
        int x = 0;
        int l = 0;
        int r = 10;
        while (l <= r)
        {
            int m = (l + r) / 2;
            Bignum t = divisor * Bignum(std::to_string(m));
            if (t <= current)
            {
                x = m;
                l = m + 1;
            }
            else
            {
                r = m - 1;
            }
        }
        result.value.insert(result.value.begin(), x);
        current = current - divisor * Bignum(std::to_string(x));
    }
    // clear all leading 0s
    while (result.value.size() > 1 && result.value.back() == 0)
    {
        result.value.pop_back();
    }
    return std::make_pair(result, current);
}

Bignum Bignum::operator%(const Bignum &divisor) const
{
    if (divisor.value.size() == 1 && divisor.value[0] == 0)
    {
        std::cout << "Error: division by zero." << std::endl;
        return Bignum();
    }

    return ((*this) / divisor).second;
}

Bignum Bignum::operator^(std::pair<Bignum, Bignum> exp_mod) const
{
    Bignum a = *this;
    Bignum b = exp_mod.first;
    Bignum c = exp_mod.second;
    Bignum zero = Bignum();

    if (b == zero)
    {
        return Bignum("1");
    }

    Bignum result("1");
    a = a % c;
    while (b > zero)
    {
        if ((b.value[0] % 2) == 1)
        {
            result = (result * a) % c;
        }
        b = (b / Bignum("2")).first;
        a = (a * a) % c;
    }
    return result;
}

Bignum Bignum::rsaEncrypt(const Bignum &public_exponent, const Bignum &modulus) const
{
    return (*this) ^ std::pair<Bignum, Bignum>(public_exponent, modulus);
}

Bignum Bignum::rsaDecrypt(const Bignum &private_exponent, const Bignum &modulus) const
{
    return (*this) ^ std::pair<Bignum, Bignum>(private_exponent, modulus);
}

bool Bignum::operator<(const Bignum &other) const
{
    if (value.size() != other.value.size())
    {
        return value.size() < other.value.size();
    }
    for (int i = value.size() - 1; i >= 0; i--)
    {
        if (value[i] != other.value[i])
        {
            return value[i] < other.value[i];
        }
    }
    return false;
}

bool Bignum::operator<=(const Bignum &other) const
{
    return (*this < other) || (*this == other);
}

bool Bignum::operator>(const Bignum &other) const
{
    return !(*this <= other);
}

bool Bignum::operator>=(const Bignum &other) const
{
    return !(*this < other);
}

bool Bignum::operator==(const Bignum &other) const
{
    return value == other.value;
}

std::string Bignum::to_string() const
{
    std::string result;
    for (int i = value.size() - 1; i >= 0; i--)
    {
        result += value[i] + '0';
    }
    return result;
}