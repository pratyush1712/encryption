#include <iostream>
#include <cstdlib>
#include <sstream>
#include <iomanip> // Add this include
#include "bignum.hpp"

const Bignum rsa_n = Bignum("9616540267013058477253762977293425063379243458473593816900454019721117570003248808113992652836857529658675570356835067184715201230519907361653795328462699");
const Bignum rsa_e = Bignum("65537");
const Bignum rsa_d = Bignum("4802033916387221748426181350914821072434641827090144975386182740274856853318276518446521844642275539818092186650425384826827514552122318308590929813048801");

std::pair<std::string, std::string> padString(const std::string &input, int lineNumber)
{
    std::string truncated = input.substr(0, 96);
    std::string prefix = (lineNumber < 10 ? "  " : lineNumber < 100 ? " "
                                                                    : "") +
                         std::to_string(lineNumber);
    std::string padded = prefix + truncated;
    padded.append(96 - truncated.length(), ' ');
    padded.append(prefix);

    std::string first = padded.substr(0, 51);
    std::string second = padded.substr(51, 51);

    return std::make_pair(first, second);
}

std::pair<Bignum, Bignum> stringToBignum(const std::pair<std::string, std::string> &input)
{
    std::ostringstream firstAscii, secondAscii;

    for (char ch : input.first)
    {
        firstAscii << std::setw(3) << std::setfill('0') << static_cast<int>(ch);
    }

    for (char ch : input.second)
    {
        secondAscii << std::setw(3) << std::setfill('0') << static_cast<int>(ch);
    }
    // remove leading zeroes
    while (firstAscii.str()[0] == '0')
    {
        firstAscii.str(firstAscii.str().substr(1));
    }
    while (secondAscii.str()[0] == '0')
    {
        secondAscii.str(secondAscii.str().substr(1));
    }
    return std::make_pair(Bignum(firstAscii.str()), Bignum(secondAscii.str()));
}

std::string decryptAndMergeStrings(const Bignum &first_bignum, const Bignum &second_bignum, bool prepend_zero_first = 1, bool prepend_zero_second = 1)
{
    std::string first_num_str = first_bignum.to_string();
    std::string second_num_str = second_bignum.to_string();

    if (prepend_zero_first == 0)
        first_num_str = "0" + first_num_str;
    if (prepend_zero_second == 0)
        second_num_str = "0" + second_num_str;

    std::ostringstream first_ss, second_ss;

    for (size_t i = 0; i < first_num_str.size(); i += 3)
    {
        int char_code = std::stoi(first_num_str.substr(i, 3));
        first_ss << static_cast<char>(char_code);
    }

    for (size_t i = 0; i < second_num_str.size(); i += 3)
    {
        int char_code = std::stoi(second_num_str.substr(i, 3));
        second_ss << static_cast<char>(char_code);
    }

    std::string output = first_ss.str() + second_ss.str();

    // Remove line numbers and trailing blanks
    output = output.substr(3, 96);
    output.erase(output.find_last_not_of(' ') + 1);

    return output;
}

std::pair<std::string, std::string> splitString(const std::string &input)
{
    return {input.substr(0, 51), input.substr(51, 51)};
}

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

// function to encrypt and decrypt a message (with the line number given)
void encryptDecrypt(const std::string &input, int lineNumber)
{
    bool prepend_zero_first = false;
    bool prepend_zero_second = false;
    std::pair<std::string, std::string> padded = padString(input, lineNumber);
    if (padded.first[0] == '0')
        prepend_zero_first = true;
    if (padded.second[0] == '0')
        prepend_zero_second = true;
    Bignum first_half = stringToBignum(padded).first;
    Bignum second_half = stringToBignum(padded).second;

    // encrypt
    Bignum encrypted_first_half = first_half.rsaEncrypt(rsa_e, rsa_n);
    Bignum encrypted_second_half = second_half.rsaEncrypt(rsa_e, rsa_n);

    // decrypt
    Bignum decrypted_first_half = encrypted_first_half.rsaDecrypt(rsa_d, rsa_n);
    Bignum decrypted_second_half = encrypted_second_half.rsaDecrypt(rsa_d, rsa_n);

    // convert to string
    std::string decrypted = decryptAndMergeStrings(decrypted_first_half, decrypted_second_half, prepend_zero_first, prepend_zero_second);

    std::cout << "Decrypted: " << decrypted << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <sentence to encrypt/decrypt>" << std::endl;
        return 1;
    }

    std::string input(argv[1]);
    encryptDecrypt(input, 2); // we use 1 as line number for simplicity

    return 0;
}

// decryption works fine. problem is with bignum to string.
// remember, we removed the starting zeroes. so we need to
// add them back so we can convert to ascii properly.