#include <iostream>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <thread>
#include <vector>
#include <mutex>
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

std::string decryptAndMergeStrings(const Bignum &first_bignum, const Bignum &second_bignum)
{
    std::string first_num_str = first_bignum.to_string();
    std::string second_num_str = second_bignum.to_string();

    if (first_num_str.length() % 3 != 0)
    {
        first_num_str = "0" + first_num_str;
    }
    if (second_num_str.length() % 3 != 0)
    {
        second_num_str = "0" + second_num_str;
    }

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
std::pair<std::string, std::string> e(const std::string &input, int lineNumber)
{
    std::pair<std::string, std::string> padded = padString(input, lineNumber);
    Bignum first_half = stringToBignum(padded).first;
    Bignum second_half = stringToBignum(padded).second;

    // encrypt
    Bignum encrypted_first_half = first_half.rsaEncrypt(rsa_e, rsa_n);
    Bignum encrypted_second_half = second_half.rsaEncrypt(rsa_e, rsa_n);

    return std::make_pair(encrypted_first_half.to_string(), encrypted_second_half.to_string());
}

std::string d(const Bignum &encrypted_first_half, const Bignum &encrypted_second_half)
{
    // decrypt
    Bignum decrypted_first_half = encrypted_first_half.rsaDecrypt(rsa_d, rsa_n);
    Bignum decrypted_second_half = encrypted_second_half.rsaDecrypt(rsa_d, rsa_n);

    // convert to string
    std::string decrypted = decryptAndMergeStrings(decrypted_first_half, decrypted_second_half);

    return decrypted;
}

int main(int argc, char *argv[])
{
    // Bignum first("1106075512756897693845541115782443403978043085451052642767226006958164432759348134413074997249085621409356986688646602003242847768263275335375958215687029");
    // Bignum second("6041941234439008986868866446092700359584184322890943920375576204389615565795614725594091512619740474084925538905434207584348157820684201345032821842865449");
    // std::string decrypted = d(first, second);
    // std::cout << decrypted << std::endl;
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <process>" << std::endl;
        return 1;
    }

    std::string process = argv[1];
    std::vector<std::thread> threads;

    if (process == "e")
    {
        std::vector<std::pair<std::string, std::string>> results;
        std::string input;
        int lineNumber = 0;
        while (std::getline(std::cin, input))
        {
            lineNumber++;
            results.resize(lineNumber);
            threads.push_back(std::thread([&, lineNumber]()
                                          {
                auto result = e(input, lineNumber);
                results[lineNumber - 1] = result; }));
        }
        for (auto &t : threads)
            t.join();
        for (auto &res : results)
        {
            std::cout << res.first << std::endl
                      << res.second << std::endl;
        }
    }
    else if (process == "d")
    {
        std::vector<std::string> results;
        std::string input1, input2;
        int linePairNumber = 0;
        while (std::getline(std::cin, input1))
        {
            if (!std::getline(std::cin, input2))
            {
                std::cerr << "Error: Expected an even number of lines." << std::endl;
                return 1;
            }
            linePairNumber++;
            results.resize(linePairNumber);
            if (!isUnsignedInt(input1) || !isUnsignedInt(input2))
            {
                std::cerr << "Error: Invalid input" << std::endl;
                return 1;
            }
            Bignum first_bignum(input1);
            Bignum second_bignum(input2);
            threads.push_back(std::thread([&, linePairNumber]()
                                          {
                    auto result = d(first_bignum, second_bignum);
                    results[linePairNumber - 1] = result; }));
        }
        for (auto &t : threads)
            t.join();
        for (auto &res : results)
        {
            std::cout << res << std::endl;
        }
    }

    return 0;
}