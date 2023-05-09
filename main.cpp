#include <iostream>
#include <cstdlib>
#include "bignum.hpp"

bool isUnsignedInt(const std::string &str)
{
    for (char c : str)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: bignum op number1 number2." << std::endl;
        return 1;
    }

    char op = argv[1][0];
    std::string num1_str = argv[2];
    std::string num2_str = argv[3];
    bool isNum1UnsignedInt = isUnsignedInt(num1_str);
    bool isNum2UnsignedInt = isUnsignedInt(num2_str);
    bool isOpValid = op == '+' || op == '-' || op == '*' || op == '/' || op == '%' || op == '^';

    if (!isNum1UnsignedInt || !isNum2UnsignedInt || !isOpValid)
    {
        if (!isOpValid)
        {
            std::cout << "Error: " << op << " is not a valid operator." << std::endl;
        }

        if (!isNum1UnsignedInt)
        {
            std::cout << "Error: \"" << num1_str << "\" is not an unsigned integer." << std::endl;
        }

        if (!isNum2UnsignedInt)
        {
            std::cout << "Error: \"" << num2_str << "\" is not an unsigned integer." << std::endl;
        }
        return 1;
    }

    Bignum num1(num1_str);
    Bignum num2(num2_str);
    if (op == '+')
    {
        std::cout << (num1 + num2).to_string() << std::endl;
    }
    else if (op == '-')
    {
        std::cout << (num1 - num2).to_string() << std::endl;
    }
    else if (op == '*')
    {
        std::cout << (num1 * num2).to_string() << std::endl;
    }
    else if (op == '/')
    {
        std::cout << (num1 / num2).to_string() << std::endl;
    }
    else if (op == '%')
    {
        std::cout << (num1 % num2).to_string() << std::endl;
    }
    else if (op == '^')
    {
        Bignum base = num1;
        Bignum exp = num2;
        Bignum mod(argv[4]);
        std::cout << (base ^ std::pair<Bignum, Bignum>(exp, mod)).to_string() << std::endl;
    }
    return 0;
}
